
#include <time.h>

#include "lince.h"
#include "cglm/vec4.h"


typedef struct MyLayer {
    float red, vel;
    LinceVertexArray* va;
    LinceBool show_fps;
    LinceBool show_mouse_pos;
} MyLayer;


void MyLayer_OnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    data->red = 0.0f;
    data->vel = 5e-4f;
    data->show_fps = LinceFalse;
    data->show_mouse_pos = LinceFalse;
    data->va; /* = LinceCreateVertexArray() */
}

void MyLayer_OnDetach(LinceLayer* layer) {
    printf("MyLayer detached\n");
    MyLayer* data = LinceGetLayerData(layer);
    /* LinceDeleteVertexArray(data->va) */
    free(data);
}

LinceBool MyLayer_OnKeyPressed(LinceEvent* e) {
    /* how to pass layer data here?? */
    int key = e->data.KeyPressed->keycode;
    switch (key) {
    case LinceKey_Backspace:
        printf("\b \b");
        break;
    case LinceKey_Enter:
        printf("\n");
        break;
    default:
        printf("%c", (char)key);
    }
    fflush(stdout);
    return LinceTrue;
}

void MyLayer_OnEvent(LinceLayer* layer, LinceEvent* e) {
    LinceDispatchEvent(e, LinceEventType_KeyPressed, MyLayer_OnKeyPressed);
}

void MyLayer_OnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);

    /* update background color */
    data->red += data->vel * dt;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;
    LinceRender_SetClearColor(data->red, 0.1f, blue, 1.0f);

    /* display data */
    if (LinceIsKeyPressed(LinceKey_Up)){
        data->show_fps != data->show_fps;
        printf("\n");
    }
    if (LinceIsKeyPressed(LinceKey_Down)){
        data->show_mouse_pos != data->show_mouse_pos;
        printf("\n");
    }

    float fps = 1000.0 / dt;
    if (data->show_fps){ // displays and updates frame rate every fram
        printf(" FPS: %.4g (dt: %.4g ms) ", fps, dt);
    }
    if (data->show_mouse_pos){ // prints mouse position
        printf(" Mouse: %.2f %.2f ", LinceGetMouseX(), LinceGetMouseY());
    }
    if (data->show_fps || data->show_mouse_pos){
        fflush(stdout);
        for(int i=0; i!=100; ++i) printf("\b");
        fflush(stdout);
    }
}

LinceLayer* MyLayer_Init(int n) {
    printf("MyLayer Init\n");

    MyLayer* my_layer = malloc(sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");
    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayer_OnAttach;
    layer->OnDetach = MyLayer_OnDetach;
    layer->OnEvent = MyLayer_OnEvent;
    layer->OnUpdate = MyLayer_OnUpdate;

    return layer;
}


void GameInit() {
	printf("Game initialised!\n");
    LincePushLayer(MyLayer_Init(1));
    LincePushLayer(MyLayer_Init(2));
}

void GameOnUpdate(float dt) {


    /*
    // Idea for 2D renderer API
    LinceBeginScene(camera);
    LinceDrawQuad({});
    LinceEndScene();
    */
}

void GameOnEvent(LinceEvent* e) {

}


void GameTerminate() {
    printf("Game terminated\n");
}

int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    printf(" --- DEBUG MODE -- \n");
    #endif

    LinceSetGameInitFn(GameInit);
    LinceSetGameOnUpdateFn(GameOnUpdate);
    LinceSetGameOnEventFn(GameOnEvent);
    LinceSetGameTerminateFn(GameTerminate);
    
    LinceRun();

    return 0;
}