

#include "lince.h"

typedef struct MyLayer {
    int dummy;
} MyLayer;


void MyLayer_OnAttach(LinceLayer* layer) {
    int data = ((MyLayer*)layer->data.GenericLayer)->dummy;
    printf("MyLayer attached (data = %d)\n", data);
}

void MyLayer_OnDetach(LinceLayer* layer) {
    printf("MyLayer detached\n");
    free(layer->data.GenericLayer);
    free(layer);
}

unsigned int MyLayer_OnKeyPressed(Event* e) {
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
    return 1;
}

void MyLayer_OnEvent(LinceLayer* layer, Event* e) {
    LinceEvent_Dispatch(e, EventType_KeyPressed, MyLayer_OnKeyPressed);
}

LinceLayer* MyLayer_Init(int n) {
    printf("MyLayer Init\n");

    MyLayer* my_layer = malloc(sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");
    my_layer->dummy = n;

    LinceLayer* layer = LinceLayer_Create(my_layer);
    layer->OnAttach = MyLayer_OnAttach;
    layer->OnDetach = MyLayer_OnDetach;
    layer->OnEvent = MyLayer_OnEvent;
    //layer->OnUpdate = 

    return layer;
}



void GameInit() {
	printf("Game initialised!\n");
    LinceApp_PushLayer(MyLayer_Init(1));
    LinceApp_PushLayer(MyLayer_Init(2));
}

void GameOnUpdate() {

}

void GameOnEvent(Event* e) {

}


void GameTerminate() {
    printf("Game terminated\n");
}

int main(int argc, const char* argv[]) {

    Lince_SetGameInitFn(GameInit);
    Lince_SetGameOnUpdateFn(GameOnUpdate);
    Lince_SetGameOnEventFn(GameOnEvent);
    Lince_SetGameTerminateFn(GameTerminate);
    
    LinceApp_Run();

    return 0;
}