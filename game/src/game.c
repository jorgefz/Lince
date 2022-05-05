
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
    LinceShader* shader;
    LinceTexture* tex_front;
    LinceTexture* tex_back;
    vec4 color;
    vec2 xyoffset;
} MyLayer;

void MyLayerOnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);

    static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        // positions    texture     color RGBA
        -0.5f, -0.5f,   0.0, 0.0,   1.0, 0.0, 0.0, 1.0,
         0.5f, -0.5f,   1.0, 0.0,   1.0, 1.0, 0.0, 1.0,
         0.5f,  0.5f,   1.0, 1.0,   0.0, 1.0, 0.0, 1.0,
        -0.5f,  0.5f,   0.0, 1.0,   0.0, 0.0, 1.0, 1.0,
    };
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float2, "aTexCoord"},
        {LinceBufferType_Float4, "aColor"},
    };

    data->ib = LinceCreateIndexBuffer(indices, 6);
    data->vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    data->va = LinceCreateVertexArray(data->ib);

    LinceBindVertexArray(data->va);
    LinceBindIndexBuffer(data->ib);

    unsigned int elems = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(data->va, data->vb, layout, elems);

    // Shader
    data->shader = LinceCreateShader("TestShader",
        "lince/assets/test.vert.glsl", "lince/assets/test.frag.glsl");
    LinceBindShader(data->shader);

    // texture test
    data->tex_front = LinceCreateTexture("Patrick", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("Patrick", "lince/assets/back.png");
    // assign textures to slots
    LinceBindTexture(data->tex_front, 0);
    LinceBindTexture(data->tex_back, 1);
    
    data->red = 0.0f;
    data->vel = 5e-4f;
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteVertexBuffer(data->vb);
    LinceDeleteIndexBuffer(data->ib);
    LinceDeleteVertexArray(data->va);
    LinceDeleteShader(data->shader);
    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);

    free(data);
}

/*
Q: increases red
A: decreases red

W: increases green
S: decreases green

E: increases blue
D: decreases blue
*/
LinceBool GameKeyPress(LinceEvent* e){
    LinceLayer* layer = LinceGetCurrentLayer();
    MyLayer* data = LinceGetLayerData(layer);

    int code = e->data.KeyPressed->keycode;
    float step = 0.03f;
    switch(code){
    case LinceKey_q: data->color[0]+=step; break;
    case LinceKey_a: data->color[0]-=step; break;
    case LinceKey_w: data->color[1]+=step; break;
    case LinceKey_s: data->color[1]-=step; break;
    case LinceKey_e: data->color[2]+=step; break;
    case LinceKey_d: data->color[2]-=step; break;
    default: return LinceFalse;
    }
    LinceSetShaderUniformVec4(data->shader, "add_color", data->color);
    return LinceFalse;
}

void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e) {
    LinceDispatchEvent(e, LinceEventType_KeyPressed, GameKeyPress);
}

void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);

    data->xyoffset[0] = -0.5;
    data->xyoffset[1] = -0.5;
    LinceSetShaderUniformVec2(data->shader, "xyoffset", data->xyoffset);
    LinceSetShaderUniformInt(data->shader, "textureID", 0);
    LinceDrawIndexed(data->shader, data->va, data->ib);

    data->xyoffset[0] = 0.5;
    data->xyoffset[1] = 0.5;
    LinceSetShaderUniformVec2(data->shader, "xyoffset", data->xyoffset);
    LinceSetShaderUniformInt(data->shader, "textureID", 1);
    LinceDrawIndexed(data->shader, data->va, data->ib);

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


void GameInit() {
	LINCE_INFO("\n User App Initialised");
    LincePushLayer(MyLayerInit("Test"));
}


void GameOnUpdate(float dt) {
    LinceCheckErrors();
}


void GameOnEvent(LinceEvent* e) {
    
}

void GameTerminate() {
    
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
