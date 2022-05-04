
#include <time.h>

#include "lince.h"
#include "cglm/vec4.h"

//#include <GLFW/glfw3.h>
//#include <glad/glad.h>


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

    LinceCheckErrors();

    /* update background color */
    data->red += data->vel * dt;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;
    LinceSetClearColor(data->red, 0.1f, blue, 1.0f);

}

LinceLayer* MyLayerInit(char* name) {

    MyLayer* my_layer = calloc(1, sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len = strlen(name) < LINCE_NAME_MAX ? strlen(name) : LINCE_NAME_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayerOnAttach;
    layer->OnDetach = MyLayerOnDetach;
    layer->OnEvent = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================

LinceIndexBuffer global_ib = {0};
LinceVertexArray *global_va = NULL;
LinceShader *global_shader = NULL;
vec4 global_color = {0};


void GameInit() {
	LINCE_INFO("\n User App Initialised");

    LincePushLayer(MyLayerInit("Test"));

    static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        // positions    color RGBA
        -0.5f, -0.5f,   1.0, 0.0, 0.0, 1.0,
         0.5f, -0.5f,   1.0, 1.0, 0.0, 1.0,
         0.5f,  0.5f,   0.0, 1.0, 0.0, 1.0,
        -0.5f,  0.5f,   0.0, 0.0, 1.0, 1.0,
    };

    global_ib = LinceCreateIndexBuffer(indices, 6);
    LinceVertexBuffer vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    global_va = LinceCreateVertexArray(global_ib);

    LinceBindVertexArray(global_va);
    LinceBindIndexBuffer(global_ib);

    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float4, "aColor"},
    };
    unsigned int elems = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(global_va, vb, layout, elems);

    // Shader
    global_shader = LinceCreateShader("TestShader",
        "lince/assets/test.vert.glsl", "lince/assets/test.frag.glsl");
    LinceBindShader(global_shader);

}


void GameOnUpdate(float dt) {
    LinceDrawIndexed(global_shader, global_va, global_ib);
}


/*
Q: increases red
A: decreases red

W: increases green
S: decreases green

E: increases blue
D: decreases blue
*/
void GameKeyPress(LinceEvent* e){
    int code = e->data.KeyPressed->keycode;
    float step = 0.03f;

    switch(code){
    case LinceKey_q: global_color[0]+=step; break;
    case LinceKey_a: global_color[0]-=step; break;
    case LinceKey_w: global_color[1]+=step; break;
    case LinceKey_s: global_color[1]-=step; break;
    case LinceKey_e: global_color[2]+=step; break;
    case LinceKey_d: global_color[2]-=step; break;
    default: return;
    }

    LinceSetShaderUniformVec4(global_shader, "add_color", global_color);
}

void GameOnEvent(LinceEvent* e) {
    LinceDispatchEvent(e, LinceEventType_KeyPressed, GameKeyPress);
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
