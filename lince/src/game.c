
#include <time.h>

#include "lince.h"
#include "cglm/vec4.h"


typedef struct MyLayer {
    float red, vel;
} MyLayer;

LinceBool show_fps = LinceFalse;

void MyLayer_OnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    data->red = 0.0f;
    data->vel = 0.001f;
}

void MyLayer_OnDetach(LinceLayer* layer) {
    printf("MyLayer detached\n");
    free(LinceGetLayerData(layer));
}

LinceBool MyLayer_OnKeyPressed(LinceEvent* e) {
    int key = e->data.KeyPressed->keycode;
    switch (key) {
    case LinceKey_Backspace:
        printf("\b \b");
        break;
    case LinceKey_Enter:
        printf("\n");
        break;
    case LinceKey_Up:
        show_fps = !show_fps;
        printf("\n");
        break;
    default:
        printf("%c", (char)key);
    }
    fflush(stdout);
    return LinceTrue;
}

void MyLayer_OnEvent(LinceLayer* layer, LinceEvent* e) {
    LinceEvent_Dispatch(e, LinceEventType_KeyPressed, MyLayer_OnKeyPressed);
}

void MyLayer_OnUpdate(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);

    data->red += data->vel;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;

    LinceRender_SetClearColor(data->red, 0.1f, blue, 1.0f);
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

double time_ms = 0.0;
double dt = 0.0;
double fps = 0.0;

void GameOnUpdate() {
    double old_time = time_ms;
    time_ms = LinceGetTimeMillis();
    
    dt = (time_ms - old_time); // delta time in ms
    fps = 1000.0 / dt;

    if (show_fps){ // displays and updates frame rate every fram
        printf(" FPS: %.4g (dt: %.4g ms) ", fps, dt);
        fflush(stdout);
        for(int i=0; i!=100; ++i) printf("\b");
        fflush(stdout);
    }
    if (LinceIsKeyPressed(LinceKey_m)){ // prints mouse position
        printf("Mouse: %.2f %.2f ", LinceGetMouseX(), LinceGetMouseY());
        fflush(stdout);
        for(int i=0; i!=100; ++i) printf("\b");
        fflush(stdout);
    }
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