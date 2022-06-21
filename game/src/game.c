
#include <time.h>

#include "lince.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

//#include <GLFW/glfw3.h>
//#include <glad/glad.h>

typedef struct MyLayer {
    char name[LINCE_NAME_MAX];
    float red, vel;
    float dt;

    LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;
    LinceShader* shader;
    LinceTexture* tex_front;
    LinceTexture* tex_back;
    vec4 color;
    LinceCamera* cam;
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

    data->cam = LinceCreateCamera(LinceGetAspectRatio());
    LinceSetShaderUniformMat4(data->shader, "u_view_proj", data->cam->view_proj);

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
    LinceDeleteCamera(data->cam);

    free(data);
}

/*
Q, A: increase, decrease red
W, S: increase, decrease green
E, D: increase, decrease blue
*/
LinceBool GameKeyPress(LinceEvent* e){
    MyLayer* data = LinceGetLayerData(LinceGetCurrentLayer());

    int code = e->data.KeyPressed->keycode;
    const float step = 0.03f;
    const float cam_speed = 0.1f;
    const float dt = data->dt, zoom = data->cam->zoom;

    switch(code){
    /* change colors */
    case LinceKey_q: data->color[0]+=step; break;
    case LinceKey_a: data->color[0]-=step; break;
    case LinceKey_w: data->color[1]+=step; break;
    case LinceKey_s: data->color[1]-=step; break;
    case LinceKey_e: data->color[2]+=step; break;
    case LinceKey_d: data->color[2]-=step; break;
    /* camera movement */
    case LinceKey_Up:    data->cam->pos[1] += cam_speed*dt*zoom; break;
    case LinceKey_Down:  data->cam->pos[1] -= cam_speed*dt*zoom; break;
    case LinceKey_Right: data->cam->pos[0] += cam_speed*dt*zoom; break;
    case LinceKey_Left:  data->cam->pos[0] -= cam_speed*dt*zoom; break;
    /* camera zoom */
    case LinceKey_Period: data->cam->zoom += 0.1; break;
    case LinceKey_Comma:  data->cam->zoom -= 0.1; break;
    default: return LinceFalse;
    }
    LinceSetShaderUniformVec4(data->shader, "add_color", data->color);
    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
    return LinceFalse;
}

LinceBool GameWindowResize(LinceEvent* e){
    MyLayer* data = LinceGetLayerData(LinceGetCurrentLayer());
    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
}

void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e) {
    LinceDispatchEvent(e, LinceEventType_KeyPressed, GameKeyPress);
    LinceDispatchEvent(e, LinceEventType_WindowResize, GameWindowResize);
}

void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);
    data->dt = dt;

    LinceUpdateCamera(data->cam);
    LinceSetShaderUniformMat4(data->shader, "u_view_proj", data->cam->view_proj);
    mat4 transform;

    vec4 pos1 = {-0.5, -0.5, 0.0, 1.0};
    glm_translate_to(GLM_MAT4_IDENTITY, pos1, transform);
    LinceSetShaderUniformMat4(data->shader, "u_transform", transform);
    LinceSetShaderUniformInt(data->shader, "textureID", 0);
    LinceDrawIndexed(data->shader, data->va, data->ib);

    vec4 pos2 = {0.5, 0.5, 0.0, 1.0};
    glm_translate_to(GLM_MAT4_IDENTITY, pos2, transform);
    LinceSetShaderUniformMat4(data->shader, "u_transform", transform);
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

    size_t len_orig = strlen(name);
    size_t len = len_orig < LINCE_NAME_MAX ? len_orig : LINCE_NAME_MAX;
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
