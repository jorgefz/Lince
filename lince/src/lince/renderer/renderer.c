#include <stdlib.h>
#include "core/profiler.h"
#include "core/memory.h"
#include "renderer/renderer.h"
#include "renderer/camera.h"
#include <glad/glad.h>
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"
#include "cglm/io.h"


#define QUAD_VERTEX_COUNT 4 // number of vertices in one quad
#define QUAD_INDEX_COUNT 6  // number of indices required to index one quad

#define MAX_QUADS 20000    // Maximum number of quads in a single vertex batch
#define MAX_VERTICES (MAX_QUADS * QUAD_VERTEX_COUNT) // max number of vertices in a batch
#define MAX_INDICES (MAX_QUADS * QUAD_INDEX_COUNT)   // max number of indices in a batch
#define MAX_TEXTURE_SLOTS 32   // max number of textures the GPU can bind simultaneously


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


/* Calculates Z order from Y coordinate */
float LinceYSortedZ(float y, vec2 ylim, vec2 zlim){
    float ynorm = (y - ylim[0]) / (ylim[1]-ylim[0]);
    float z = (zlim[0] - zlim[1]) * ynorm + zlim[1];
    return z;
}

// stores information of one vertex
// Should be packed because everything is a float
typedef struct LinceQuadVertex {
	float x, y, z; 	   // position
	float s, t; 	   // texture coordinates
	float color[4];	   // rgba color
	float texture_id;  // binding slot for the texture
} LinceQuadVertex;

typedef struct LinceRendererState {
	LinceShader *default_shader, *shader;
	LinceTexture* white_texture;
	
	LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;

	// Batch rendering
	unsigned int quad_count;       // number of quads in the batch
	LinceQuadVertex* vertex_batch; // collection of vertices to render
	unsigned int* index_batch;     // collection of indices to render

	unsigned int texture_slot_count;
	LinceTexture* texture_slots[MAX_TEXTURE_SLOTS];

} LinceRendererState;

/* Global rendering state */
static LinceRendererState renderer_state = {0};

/* quad of size 1x1 centred on 0,0 */
static const LinceQuadVertex quad_vertices[4] = {
	{.x=-0.5f, .y=-0.5f, .z=0, .s=0.0, .t=0.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x= 0.5f, .y=-0.5f, .z=0, .s=1.0, .t=0.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x= 0.5f, .y= 0.5f, .z=0, .s=1.0, .t=1.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
	{.x=-0.5f, .y= 0.5f, .z=0, .s=0.0, .t=1.0, .color={0.0, 0.0, 0.0, 1.0}, .texture_id=0.0},
};
static const unsigned int quad_indices[] = {0,1,2,2,3,0};


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
	renderer_state.vertex_batch = LinceCalloc(MAX_VERTICES*sizeof(LinceQuadVertex));
	renderer_state.index_batch = LinceCalloc(MAX_INDICES*sizeof(unsigned int));
	
	renderer_state.vb = LinceCreateVertexBuffer(
		renderer_state.vertex_batch,
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
			renderer_state.index_batch[i + j] = offset + quad_indices[j];
		}
		offset += QUAD_VERTEX_COUNT;
	}

	renderer_state.ib = LinceCreateIndexBuffer(renderer_state.index_batch, MAX_INDICES);
	renderer_state.va = LinceCreateVertexArray(renderer_state.ib);
	LinceBindVertexArray(renderer_state.va);
    LinceBindIndexBuffer(renderer_state.ib);
	unsigned int elem_count = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(
		renderer_state.va,
		renderer_state.vb,
		layout, elem_count
	);
	
	// create default white texture
	renderer_state.white_texture = LinceCreateEmptyTexture(1, 1);
	static unsigned char white_pixel[] = {0xFF, 0xFF, 0xFF, 0xFF};
	LinceSetTextureData(renderer_state.white_texture, white_pixel);
	LinceBindTexture(renderer_state.white_texture, 0);
	
	renderer_state.default_shader = LinceCreateShaderFromSrc(
		default_vertex_source,
		default_fragment_source
	);
    LinceBindShader(renderer_state.default_shader);

	int samplers[MAX_TEXTURE_SLOTS] = { 0 };
	for (int i = 0; i != MAX_TEXTURE_SLOTS; ++i) samplers[i] = i;
	LinceSetShaderUniformIntN(renderer_state.default_shader, "uTextureSlots", samplers, MAX_TEXTURE_SLOTS);
	renderer_state.shader = renderer_state.default_shader;

	LINCE_PROFILER_END(timer);
}

void LinceTerminateRenderer() {
	renderer_state.quad_count = 0;
	if(renderer_state.vertex_batch){
		LinceFree(renderer_state.vertex_batch);
		renderer_state.vertex_batch = NULL;
	}
	if(renderer_state.index_batch){
		LinceFree(renderer_state.index_batch);
		renderer_state.index_batch = NULL;
	}

	LinceDeleteShader(renderer_state.default_shader);
    LinceDeleteTexture(renderer_state.white_texture);

	LinceDeleteVertexBuffer(renderer_state.vb);
    LinceDeleteIndexBuffer(renderer_state.ib);
    LinceDeleteVertexArray(renderer_state.va);
}

void LinceBeginScene(LinceCamera* cam) {
	LINCE_PROFILER_START(timer);

	/* Bind OpenGL objects */
	LinceBindShader(renderer_state.default_shader);
	LinceBindVertexArray(renderer_state.va);
    LinceBindIndexBuffer(renderer_state.ib);

	/* Update settings */
	LinceEnableAlphaBlend();
	LinceEnableDepthTest();

	/* Update camera */
	LinceSetShaderUniformMat4(renderer_state.default_shader,
		"u_view_proj", cam->view_proj);
	
	/* Reset batch */
	renderer_state.quad_count = 0;
	renderer_state.texture_slots[0] = renderer_state.white_texture;
	renderer_state.texture_slot_count = 1;

	size_t size = (MAX_VERTICES * sizeof(LinceQuadVertex));
	memset(renderer_state.vertex_batch, 0, size);

	LINCE_PROFILER_END(timer);
}

void LinceFlushScene(){
	LINCE_PROFILER_START(timer);

	for (uint32_t i = 0; i != renderer_state.texture_slot_count; ++i){
		LinceBindTexture(renderer_state.texture_slots[i], i);
	}
	LinceDrawIndexed(renderer_state.shader, renderer_state.va, renderer_state.ib);
	
	LINCE_PROFILER_END(timer);
}


static int LinceCompareQuadsBlendOrder(const void *a, const void *b){
	float alpha1 = ((const LinceQuadVertex*)(a))->color[3];
	float alpha2 = ((const LinceQuadVertex*)(b))->color[3];
	// If both opaque or both translucent, sort by z order
	if ((alpha1 == 1.0 && alpha2 == 1.0) || (alpha1 < 1.0 && alpha2 < 1.0)){
		float z1 = ((const LinceQuadVertex*)(a))->z;
		float z2 = ((const LinceQuadVertex*)(b))->z;
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
static void LinceSortQuadsForBlending(){
	LinceQuadVertex *batch = renderer_state.vertex_batch;
	uint32_t count = renderer_state.quad_count;
	qsort(batch, count, sizeof(LinceQuadVertex)*4, LinceCompareQuadsBlendOrder);
}


void LinceEndScene() {
	LinceSortQuadsForBlending();
	uint32_t size = (uint32_t)(MAX_VERTICES * sizeof(LinceQuadVertex));
	LinceSetVertexBufferData(renderer_state.vb, renderer_state.vertex_batch, size);
	LinceFlushScene();
}

void LinceStartNewBatch(){
	LinceEndScene();
	renderer_state.quad_count = 0;
	renderer_state.texture_slots[0] = renderer_state.white_texture;
	renderer_state.texture_slot_count = 1;
}

void LinceDrawSprite(LinceSprite* sprite, LinceShader* shader) {
	LINCE_PROFILER_START(timer);

	// Choose shader
	if(!shader){
		shader = renderer_state.default_shader;
		LinceBindShader(renderer_state.default_shader);
	} else if (shader != renderer_state.shader){
		LinceStartNewBatch();
		LinceBindShader(renderer_state.shader);
	}
	renderer_state.shader = shader;

	// batch size check
	if (renderer_state.quad_count >= MAX_QUADS ||
		renderer_state.texture_slot_count >= MAX_TEXTURE_SLOTS)
	{
		LinceStartNewBatch();
	}
	
	// calculate texture index
	float texture_index = 0.0f;
	
	if(sprite->texture){
		uint32_t slots = renderer_state.texture_slot_count;

		// check if texture already in slots
		for(uint32_t i = 0; i != slots; ++i){
			if(sprite->texture != renderer_state.texture_slots[i]) continue;
			texture_index = (float)i;
			break;
 		}
		// otherwise add texture
		if(texture_index <= 0.5f){
			renderer_state.texture_slots[slots] = sprite->texture;
			renderer_state.texture_slot_count++;
			texture_index = (float)slots;
		}
	}

	// calculate transform
	mat4 transform = GLM_MAT4_IDENTITY_INIT;
	vec4 pos = {sprite->x, sprite->y, sprite->zorder, 1.0};
	vec3 scale = {sprite->w, sprite->h, 1.0};
    glm_translate(transform, pos);
	glm_rotate(transform, glm_rad(sprite->rotation), (vec3){0.0, 0.0, -1.0});
    glm_scale(transform, scale);

	// append transformed vertices to batch
	for (uint32_t i = 0; i != QUAD_VERTEX_COUNT; ++i) {
		LinceQuadVertex vertex = {0};
		vec4 vpos = {quad_vertices[i].x, quad_vertices[i].y, 0.0, 1.0};
		vec4 res;
		glm_mat4_mulv(transform, vpos, res);
		vertex.x = res[0];
		vertex.y = res[1];
		vertex.z = res[2];

		if(sprite->tile){
			vertex.s = sprite->tile->coords[i*2];
			vertex.t = sprite->tile->coords[i*2 + 1];
		} else {
			vertex.s = quad_vertices[i].s;
			vertex.t = quad_vertices[i].t;
		}

		vertex.texture_id = texture_index;
		memcpy(vertex.color, sprite->color, sizeof(float)*4);
		size_t offset = renderer_state.quad_count * QUAD_VERTEX_COUNT + i;
		memcpy(renderer_state.vertex_batch + offset, &vertex, sizeof(vertex));
	}
	renderer_state.quad_count++;

	LINCE_PROFILER_END(timer);
}

