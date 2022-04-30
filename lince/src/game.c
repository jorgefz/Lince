
#include <time.h>

#include "lince.h"
#include "renderer/shader.h"
#include "cglm/vec4.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

static const char* vertex_shader_source =
"#version 330\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"out vec4 vColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0, 1.0);\n"
"   vColor = aColor;\n"
"}\n";
 
static const char* fragment_shader_source =
"#version 330\n"
"out vec4 color;\n"
"in vec4 vColor;\n"
"void main()\n"
"{\n"
//"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"    color = vColor;\n"
"}\n";


typedef struct MyLayer {
    char name[LINCE_NAME_MAX];
    float red, vel;

    LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;
} MyLayer;

void MyLayerOnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);
    
    data->red = 0.0f;
    data->vel = 5e-4f;
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);
    free(data);
}


void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e) {

}

void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        LINCE_ASSERT(LinceFalse, "GLerror: %d", err);
    }

    /* update background color */
    data->red += data->vel * dt;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;
    LinceRender_SetClearColor(data->red, 0.1f, blue, 1.0f);

}

LinceLayer* MyLayerInit(char* name) {

    MyLayer* my_layer = calloc(1, sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len = strlen(name) < LINCE_STR_MAX ? strlen(name) : LINCE_STR_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayerOnAttach;
    layer->OnDetach = MyLayerOnDetach;
    layer->OnEvent = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================

LinceVertexArray *global_va = NULL;
LinceShader *global_shader = NULL;

void GameInit() {
	LINCE_INFO("\n User App Initialised");

    LincePushLayer(MyLayerInit("Test"));

    static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        -0.5f, -0.5f,   1.0, 0.0, 0.0, 1.0,
         0.5f, -0.5f,   1.0, 1.0, 0.0, 1.0,
         0.5f,  0.5f,   0.0, 1.0, 0.0, 1.0,
        -0.5f,  0.5f,   0.0, 0.0, 1.0, 1.0,
    };

    LinceIndexBuffer ib = LinceCreateIndexBuffer(indices, 6);
    LinceVertexBuffer vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    global_va = LinceCreateVertexArray(ib);

    LinceBindVertexArray(global_va);
    LinceBindIndexBuffer(ib);
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float4, "aColor"},
    };
    unsigned int elems = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(global_va, vb, layout, elems);

    // Shader
    global_shader = LinceCreateShaderFromSrc(
        "TestShader", vertex_shader_source, fragment_shader_source);
    LinceBindShader(global_shader);
}


void GameOnUpdate(float dt) {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void GameOnEvent(LinceEvent* e) {

}


void GameTerminate() {
    LinceDeleteVertexArray(global_va);
    LinceDeleteShader(global_shader);
    LINCE_INFO(" User App Terminated");
}


int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    LinceSetGameInitFn(GameInit);
    LinceSetGameOnUpdateFn(GameOnUpdate);
    LinceSetGameOnEventFn(GameOnEvent);
    LinceSetGameTerminateFn(GameTerminate);
    
    LinceRun();

    return 0;
}
