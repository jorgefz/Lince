#include "renderer/renderer.h"
#include "renderer/camera.h"
#include <glad/glad.h>
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

typedef struct LinceRenderer {
	struct LinceSceneData {
		void* data;
	} scene_data;
} LinceRenderer;

static LinceRendererState renderer_state = {0};


LinceRendererState* LinceGetRendererState(){
	return &renderer_state;
}


void LinceInitRenderer(LinceWindow* window) {

	// Add up alpha channels
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialise geometry
	static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        // positions    texture     color RGBA
        -1.0f, -1.0f,   0.0, 0.0,   0.0, 0.0, 0.0, 1.0,
         1.0f, -1.0f,   1.0, 0.0,   0.0, 0.0, 0.0, 1.0,
         1.0f,  1.0f,   1.0, 1.0,   0.0, 0.0, 0.0, 1.0,
        -1.0f,  1.0f,   0.0, 1.0,   0.0, 0.0, 0.0, 1.0,
    };
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float2, "aTexCoord"},
        {LinceBufferType_Float4, "aColor"},
    };
	
	renderer_state.ib = LinceCreateIndexBuffer(indices, sizeof(indices)/sizeof(unsigned int));
    renderer_state.vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    renderer_state.va = LinceCreateVertexArray(renderer_state.ib);
	LinceBindVertexArray(renderer_state.va);
    LinceBindIndexBuffer(renderer_state.ib);
	unsigned int elems = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(
		renderer_state.va,
		renderer_state.vb,
		layout, elems
	);

	// Initialise shader
    renderer_state.shader = LinceCreateShader("RendererShader",
        "lince/assets/test.vert.glsl", "lince/assets/test.frag.glsl");
    LinceBindShader(renderer_state.shader);

	// Initialise default white texture
	renderer_state.white_texture = LinceCreateEmptyTexture("WhiteTexture", 1, 1);
	static unsigned char white_pixel[] = {0xFF, 0xFF, 0xFF, 0xFF};
	LinceSetTextureData(renderer_state.white_texture, white_pixel);
	LinceBindTexture(renderer_state.white_texture, 0);

	// Initialise 2D camera
	float aspect_ratio = (float)window->width / (float)window->height;
	renderer_state.window = window;
	renderer_state.cam = LinceCreateCamera(aspect_ratio);
    LinceSetShaderUniformMat4(
		renderer_state.shader,
		"u_view_proj",
		renderer_state.cam->view_proj
	);
}

void LinceTerminateRenderer() {
	LinceDeleteCamera(renderer_state.cam);
	LinceDeleteShader(renderer_state.shader);
    LinceDeleteTexture(renderer_state.white_texture);

	LinceDeleteVertexBuffer(renderer_state.vb);
    LinceDeleteIndexBuffer(renderer_state.ib);
    LinceDeleteVertexArray(renderer_state.va);
}

void LinceBeginScene() {
	/* Update camera */
	LinceWindow* w = renderer_state.window;
	float aspect_ratio = (float)w->width / (float)w->height;
	LinceResizeCameraView(renderer_state.cam, aspect_ratio);
	LinceUpdateCamera(renderer_state.cam);
	LinceSetShaderUniformMat4(renderer_state.shader, "u_view_proj", renderer_state.cam->view_proj);
}

void LinceEndScene() {
	// flush batch
}

void LinceDrawQuad(LinceQuadProps props) {
	// sort out default values
	//if (props.w <= 0.0) props.w = 1.0;
	//if (props.h <= 0.0) props.h = 1.0;

	// calculate transform
	mat4 transform;
	vec4 pos = {props.x, props.y, 0.0, 0.0};
	vec3 scale = {props.w, props.h, 1.0};
    glm_translate_to(GLM_MAT4_IDENTITY, pos, transform);
    glm_scale(transform, scale);
	LinceSetShaderUniformMat4(renderer_state.shader, "u_transform", transform);

	// texture
	int tex_id = 0; // default white texture
	if (props.texture){
		LinceBindTexture(props.texture, 1);
		tex_id = 1;
	}
	LinceSetShaderUniformInt(renderer_state.shader, "textureID", tex_id);
	LinceSetShaderUniformVec4(renderer_state.shader, "add_color", props.color);

	// for now, don't batch
	LinceDrawIndexed(renderer_state.shader, renderer_state.va, renderer_state.ib);
}


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
