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

// stores information of one vertex
// ensure this struct has no padding
typedef struct LinceQuadVertex {
	float x, y, z; 	   // position
	float s, t; 	   // texture coordinates
	float color[4];	   // rgba color
	float texture_id;  // binding slot for the texture
} LinceQuadVertex;

typedef struct LinceRendererState {
	LinceShader* shader;
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
	LinceBindShader(shader);
	LinceBindIndexBuffer(ib);
	LinceBindVertexArray(va);
	glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, 0);
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

	LinceEnableAlphaBlend();
	LinceEnableDepthTest();

	// Initialise geometry
	renderer_state.vertex_batch = calloc(MAX_VERTICES, sizeof(LinceQuadVertex));
	renderer_state.index_batch = calloc(MAX_INDICES, sizeof(unsigned int));
	LINCE_ASSERT_ALLOC(renderer_state.vertex_batch, sizeof(LinceQuadVertex) * MAX_VERTICES);
	LINCE_ASSERT_ALLOC(renderer_state.index_batch, sizeof(unsigned int) * MAX_INDICES);
	
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
	renderer_state.white_texture = LinceCreateEmptyTexture("WhiteTexture", 1, 1);
	static unsigned char white_pixel[] = {0xFF, 0xFF, 0xFF, 0xFF};
	LinceSetTextureData(renderer_state.white_texture, white_pixel);
	LinceBindTexture(renderer_state.white_texture, 0);

	// create shader
	renderer_state.shader = LinceCreateShader(
		"RendererShader",
        "lince/assets/test.vert.glsl",
		"lince/assets/test.frag.glsl"
	);
    LinceBindShader(renderer_state.shader);

	int samplers[MAX_TEXTURE_SLOTS] = { 0 };
	for (int i = 0; i != MAX_TEXTURE_SLOTS; ++i) samplers[i] = i;
	LinceSetShaderUniformIntN(renderer_state.shader, "uTextureSlots", samplers, MAX_TEXTURE_SLOTS);
}

void LinceTerminateRenderer() {
	renderer_state.quad_count = 0;
	if(renderer_state.vertex_batch){
		free(renderer_state.vertex_batch);
		renderer_state.vertex_batch = NULL;
	}
	if(renderer_state.index_batch){
		free(renderer_state.index_batch);
		renderer_state.index_batch = NULL;
	}

	LinceDeleteShader(renderer_state.shader);
    LinceDeleteTexture(renderer_state.white_texture);

	LinceDeleteVertexBuffer(renderer_state.vb);
    LinceDeleteIndexBuffer(renderer_state.ib);
    LinceDeleteVertexArray(renderer_state.va);
}

void LinceBeginScene(LinceCamera* cam) {
	/* Bind OpenGL objects */
	LinceBindShader(renderer_state.shader);
	LinceBindVertexArray(renderer_state.va);
    LinceBindIndexBuffer(renderer_state.ib);

	/* Update settings */
	LinceEnableAlphaBlend();
	LinceEnableDepthTest();

	/* Update camera */
	LinceSetShaderUniformMat4(renderer_state.shader, "u_view_proj", cam->view_proj);
	
	/* Reset batch */
	renderer_state.quad_count = 0;
	renderer_state.texture_slots[0] = renderer_state.white_texture;
	renderer_state.texture_slot_count = 1;
}

void LinceFlushScene(){
	
	for (uint32_t i = 0; i != renderer_state.texture_slot_count; ++i){
		LinceBindTexture(renderer_state.texture_slots[i], i);
	}
	LinceDrawIndexed(renderer_state.shader, renderer_state.va, renderer_state.ib);
	
}

void LinceEndScene() {
	uint32_t size = (uint32_t)(renderer_state.quad_count * sizeof(LinceQuadVertex) * QUAD_VERTEX_COUNT);
	LinceSetVertexBufferData(renderer_state.vb, renderer_state.vertex_batch, size);
	LinceFlushScene();
}

void LinceStartNewBatch(){
	LinceEndScene();
	renderer_state.quad_count = 0;
	renderer_state.texture_slots[0] = renderer_state.white_texture;
	renderer_state.texture_slot_count = 1;
}

void LinceDrawQuad(LinceQuadProps props) {

	// batch size check
	if (renderer_state.quad_count >= MAX_QUADS ||
		renderer_state.texture_slot_count >= MAX_TEXTURE_SLOTS)
	{
		LinceStartNewBatch();
	}
	
	// calculate texture index
	float texture_index = 0.0f;
	if(props.tile){
		props.texture = props.tile->texture;
	}

	if(props.texture){
		uint32_t slots = renderer_state.texture_slot_count;

		// check if texture already in slots
		for(uint32_t i = 0; i != slots; ++i){
			if(props.texture != renderer_state.texture_slots[i]) continue;
			texture_index = (float)i;
			break;
 		}
		// otherwise add texture
		if(texture_index <= 0.5f){
			renderer_state.texture_slots[slots] = props.texture;
			renderer_state.texture_slot_count++;
			texture_index = (float)slots;
		}
	}

	// calculate transform
	mat4 transform = GLM_MAT4_IDENTITY_INIT;
	vec4 pos = {props.x, props.y, props.zorder, 1.0};
	vec3 scale = {props.w, props.h, 1.0};
    glm_translate(transform, pos);
	glm_rotate(transform, glm_rad(props.rotation), (vec3){0.0, 0.0, -1.0});
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

		if(props.tile){
			vertex.s = props.tile->coords[i*2];
			vertex.t = props.tile->coords[i*2 + 1];
		} else {
			vertex.s = quad_vertices[i].s;
			vertex.t = quad_vertices[i].t;
		}

		vertex.texture_id = texture_index;
		memcpy(vertex.color, props.color, sizeof(float)*4);
		size_t offset = renderer_state.quad_count * QUAD_VERTEX_COUNT + i;
		memcpy(renderer_state.vertex_batch + offset, &vertex, sizeof(vertex));
	}
	renderer_state.quad_count++;
	
}

