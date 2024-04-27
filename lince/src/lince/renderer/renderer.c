#include <stdlib.h>
#include <glad/glad.h>

#include <cglm/types.h>
#include <cglm/vec4.h>
#include <cglm/affine.h>
#include <cglm/io.h>

#include "lince/core/profiler.h"
#include "lince/utils/memory.h"
#include "lince/renderer/renderer.h"
#include "lince/renderer/camera.h"
#include "lince/core/logger.h"


#define QUAD_VERTEX_COUNT 4 // number of vertices in one quad
#define QUAD_INDEX_COUNT 6  // number of indices required to index one quad

#define MAX_QUADS 20000    // Maximum number of quads in a single vertex batch
#define MAX_VERTICES (MAX_QUADS * QUAD_VERTEX_COUNT) // max number of vertices in a batch
#define MAX_INDICES (MAX_QUADS * QUAD_INDEX_COUNT)   // max number of indices in a batch


/* Global rendering state */
static LinceRenderState RENDER_STATE = {0};


const char default_fragment_source[] =
	"#version 450 core\n"
	"layout(location = 0) out vec4 color;\n"
	"in vec4 vColor;\n"
	"in vec2 vTexCoord;\n"
	"in float vTextureID;\n"
	"uniform sampler2D uTextureSlots[32];\n"
	"void main(){\n"
	"	color = texture(uTextureSlots[int(vTextureID)], vTexCoord) * vColor;\n"
	"	if (color.a == 0.0) discard;\n"
	"	// temporary solution for full transparency, not translucency.\n"
	"}\n";

const char default_vertex_source[] = 
	"#version 450 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec2 aTexCoord;\n"
	"layout (location = 2) in vec4 aColor;\n"
	"layout (location = 3) in float aTextureID;\n"
	"uniform mat4 u_view_proj = mat4(1.0);\n"
	"out vec4 vColor;\n"
	"out vec2 vTexCoord;\n"
	"out float vTextureID;\n"
	"void main(){\n"
	"   gl_Position = u_view_proj * vec4(aPos, 1.0);\n"
	"   vColor = aColor;\n"
	"   vTexCoord = aTexCoord;\n"
	"   vTextureID = aTextureID;\n"
	"}\n";



/** @brief Returns the global renderer state */
LinceRenderState* LinceGetRenderer(){
	return &RENDER_STATE;
}


/* Calculates Z order from Y coordinate */
float LinceYSortedZ(float y, vec2 ylim, vec2 zlim){
    float ynorm = (y - ylim[0]) / (ylim[1]-ylim[0]);
    float z = (zlim[0] - zlim[1]) * ynorm + zlim[1];
    return z;
}


/* quad of size 1x1 centred on 0,0 */
static const LinceQuadVertex QUAD_VERTICES[4] = {
	{.x=-0.5f, .y=-0.5f, .z=0, .s=0.0, .t=0.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x= 0.5f, .y=-0.5f, .z=0, .s=1.0, .t=0.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x= 0.5f, .y= 0.5f, .z=0, .s=1.0, .t=1.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x=-0.5f, .y= 0.5f, .z=0, .s=0.0, .t=1.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
};
static const unsigned int QUAD_INDICES[] = {0,1,2,2,3,0};


void LinceDrawIndexed(
	LinceShader* shader, LinceVertexArray* va, LinceIndexBuffer ib
) {
	LINCE_PROFILER_START(timer);
	LinceBindShader(shader);
	LinceBindIndexBuffer(ib);
	LinceBindVertexArray(va);
	glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, 0);
	LINCE_PROFILER_END(timer);
}

void LinceClear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LinceSetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void LinceEnableAlphaBlend(){
	// Add up alpha channels
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void LinceEnableDepthTest(){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS); // GL_LEQUAL
	//glDepthRange(0.0f, 1.0f);
}


void LinceInitRenderer() {
	LINCE_PROFILER_START(timer);
	
	LinceEnableAlphaBlend();
	LinceEnableDepthTest();

	// Initialise geometry
	RENDER_STATE.vertex_batch = LinceCalloc(MAX_VERTICES*sizeof(LinceQuadVertex));
	RENDER_STATE.index_batch = LinceCalloc(MAX_INDICES*sizeof(unsigned int));
	
	RENDER_STATE.vb = LinceCreateVertexBuffer(
		RENDER_STATE.vertex_batch,
		MAX_VERTICES * sizeof(LinceQuadVertex)
	);
	LinceBufferElement layout[] = {
        {LinceBufferType_Float3, "aPos",       0,0,0,0},
        {LinceBufferType_Float2, "aTexCoord",  0,0,0,0},
        {LinceBufferType_Float4, "aColor",     0,0,0,0},
		{LinceBufferType_Float,  "aTextureID", 0,0,0,0}
    };

	// Generate indices for all quads in a full batch
	unsigned int offset = 0;
	for (unsigned int i = 0; i != MAX_INDICES; i += QUAD_INDEX_COUNT) {
		for (unsigned int j = 0; j != QUAD_INDEX_COUNT; ++j) {
			RENDER_STATE.index_batch[i + j] = offset + QUAD_INDICES[j];
		}
		offset += QUAD_VERTEX_COUNT;
	}

	RENDER_STATE.ib = LinceCreateIndexBuffer(RENDER_STATE.index_batch, MAX_INDICES);
	RENDER_STATE.va = LinceCreateVertexArray(RENDER_STATE.ib);
	LinceBindVertexArray(RENDER_STATE.va);
    LinceBindIndexBuffer(RENDER_STATE.ib);
	unsigned int elem_count = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(
		RENDER_STATE.va,
		RENDER_STATE.vb,
		layout, elem_count
	);
	
	// create default white texture
	RENDER_STATE.white_texture = LinceCreateEmptyTexture(1, 1);
	static unsigned char white_pixel[] = {0xFF, 0xFF, 0xFF, 0xFF};
	LinceTextureSetData(RENDER_STATE.white_texture, white_pixel);
	LinceBindTexture(RENDER_STATE.white_texture, 0);
	
	RENDER_STATE.default_shader = LinceCreateShaderFromSrc(
		default_vertex_source,
		default_fragment_source
	);
    LinceBindShader(RENDER_STATE.default_shader);

	int samplers[LINCE_MAX_TEXTURE_UNITS] = { 0 };
	for (int i = 0; i != LINCE_MAX_TEXTURE_UNITS; ++i) samplers[i] = i;
	LinceSetShaderUniformIntN(RENDER_STATE.default_shader, "uTextureSlots", samplers, LINCE_MAX_TEXTURE_UNITS);
	RENDER_STATE.shader = RENDER_STATE.default_shader;

	LINCE_PROFILER_END(timer);
}

void LinceTerminateRenderer() {
	RENDER_STATE.quad_count = 0;
	if(RENDER_STATE.vertex_batch){
		LinceFree(RENDER_STATE.vertex_batch);
		RENDER_STATE.vertex_batch = NULL;
	}
	if(RENDER_STATE.index_batch){
		LinceFree(RENDER_STATE.index_batch);
		RENDER_STATE.index_batch = NULL;
	}

	LinceDeleteShader(RENDER_STATE.default_shader);
    LinceDeleteTexture(RENDER_STATE.white_texture);

	LinceDeleteVertexBuffer(RENDER_STATE.vb);
    LinceDeleteIndexBuffer(RENDER_STATE.ib);
    LinceDeleteVertexArray(RENDER_STATE.va);
}

void LinceBeginRender(LinceCamera* cam) {
	LINCE_PROFILER_START(timer);

	/* Bind OpenGL objects */
	LinceBindShader(RENDER_STATE.default_shader);
	LinceBindVertexArray(RENDER_STATE.va);
    LinceBindIndexBuffer(RENDER_STATE.ib);

	/* Update settings */
	LinceEnableAlphaBlend();
	LinceEnableDepthTest();

	/* Update camera */
	LinceSetShaderUniformMat4(RENDER_STATE.default_shader,
		"u_view_proj", cam->view_proj);
	
	/* Reset batch */
	RENDER_STATE.quad_count = 0;
	RENDER_STATE.texture_slots[0] = RENDER_STATE.white_texture;
	RENDER_STATE.texture_slot_count = 1;

	size_t size = (MAX_VERTICES * sizeof(LinceQuadVertex));
	memset(RENDER_STATE.vertex_batch, 0, size);

	LINCE_PROFILER_END(timer);
}

static void LinceDrawBatch(){
	LINCE_PROFILER_START(timer);

	for (uint32_t i = 0; i != RENDER_STATE.texture_slot_count; ++i){
		LinceBindTexture(RENDER_STATE.texture_slots[i], i);
	}
	LinceDrawIndexed(RENDER_STATE.shader, RENDER_STATE.va, RENDER_STATE.ib);
	
	LINCE_PROFILER_END(timer);
}

/*
Describes blending order for quads.
*/
static int LinceCompareQuadsBlendOrder(const void *a, const void *b){
	const LinceQuadVertex* qva = a;
	const LinceQuadVertex* qvb = b;
	float alpha1 = qva->color[3];
	float alpha2 = qvb->color[3];
	// If both opaque or both translucent, sort by z order
	if ((alpha1 == 1.0 && alpha2 == 1.0) || (alpha1 < 1.0 && alpha2 < 1.0)){
		float z1 = qva->z;
		float z2 = qvb->z;
		return z1 > z2 ? 1 : -1; // ascending
	}
	// If one opaque and one translucent,
	// always return translucent > opaque
	return alpha2 > alpha1 ? 1 : -1; // descending
}

/*
Enables depth test with translucency.
Sorts quads such that opaque ones are drawn first,
followed by translucent ones from back to front.
See https://www.opengl.org/archives/resources/faq/technical/transparency.htm
Also see https://learnopengl.com/Advanced-OpenGL/Blending
*/
static void LinceSortQuadsBlendOrder(){
	LinceQuadVertex *batch = RENDER_STATE.vertex_batch;
	uint32_t count = RENDER_STATE.quad_count;
	qsort(batch, count, sizeof(LinceQuadVertex)*4, LinceCompareQuadsBlendOrder);
}


void LinceEndRender() {
	LinceSortQuadsBlendOrder();
	uint32_t size = (uint32_t)(MAX_VERTICES * sizeof(LinceQuadVertex));
	LinceSetVertexBufferData(RENDER_STATE.vb, RENDER_STATE.vertex_batch, size);
	LinceDrawBatch();
}

void LinceFlushRender(){
	LinceEndRender();
	RENDER_STATE.quad_count = 0;
	RENDER_STATE.texture_slots[0] = RENDER_STATE.white_texture;
	RENDER_STATE.texture_slot_count = 1;
}

void LinceDrawQuad(
		LinceTransform *transform,
		LinceQuadData  *data,
		LinceTexture   *texture,
		LinceShader    *shader
	){
	LINCE_PROFILER_START(timer);

	// Choose shader
	if(!shader){
		shader = RENDER_STATE.default_shader;
	}
	if (shader != RENDER_STATE.shader){
		LinceFlushRender(); // Required when switching shaders
		RENDER_STATE.shader = shader;
	}
	LinceBindShader(shader);

	// Check if batch is too large
	if (RENDER_STATE.quad_count >= MAX_QUADS ||
		RENDER_STATE.texture_slot_count >= LINCE_MAX_TEXTURE_UNITS)
	{
		LinceFlushRender();
	}

	// Calculate texture index
	int texture_index = 0;
	if(texture){
		uint32_t slots = RENDER_STATE.texture_slot_count;
		
		// check if texture already in slots
		for(uint32_t i = 0; i != slots; ++i){
			if(texture != RENDER_STATE.texture_slots[i]){
				continue;
			}
			texture_index = (int)i;
			break;
 		}
		// otherwise add texture
		if(texture_index <= 0.5f){
			RENDER_STATE.texture_slots[slots] = texture;
			RENDER_STATE.texture_slot_count++;
			texture_index = (int)slots;
		}
	}

	// Calculate transform
	mat4 tmatrix = GLM_MAT4_IDENTITY_INIT;
	vec4 pos = {transform->x, transform->y, data->zorder, 1.0};
	vec3 scale = {transform->w, transform->h, 1.0};
    glm_translate(tmatrix, pos);
	glm_rotate(tmatrix, glm_rad(data->rotation), (vec3){0.0, 0.0, -1.0});
    glm_scale(tmatrix, scale);

	// Transform vertices and add to batch
	for (uint32_t i = 0; i != QUAD_VERTEX_COUNT; ++i) {
		LinceQuadVertex vertex = {0};
		vec4 vpos = {QUAD_VERTICES[i].x, QUAD_VERTICES[i].y, 0.0, 1.0};
		vec4 res;
		glm_mat4_mulv(tmatrix, vpos, res);
		vertex.x = res[0];
		vertex.y = res[1];
		vertex.z = res[2];
		vertex.s = data->uv[i*2];
		vertex.t = data->uv[i*2 + 1];

		vertex.texture_id = (float)texture_index;
		memcpy(vertex.color, data->color, sizeof(float)*4);
		size_t offset = RENDER_STATE.quad_count * QUAD_VERTEX_COUNT + i;
		memcpy(RENDER_STATE.vertex_batch + offset, &vertex, sizeof(vertex));
	}
	RENDER_STATE.quad_count++;

	LINCE_PROFILER_END(timer);
}

