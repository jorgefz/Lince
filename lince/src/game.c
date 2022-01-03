

#include "lince.h"

typedef struct MyLayer {
    int dummy;
} MyLayer;


void MyLayer_OnAttach(LinceLayer* layer) {
    int data = ((MyLayer*)layer->data.GenericLayer)->dummy;
    printf("MyLayer attached (data = %d)\n", data);
}

void MyLayer_OnDetach(LinceLayer* layer) {
    printf("MyLayer Destroy\n");
    free(layer->data.GenericLayer);
    free(layer);
}

LinceLayer* MyLayer_Init() {
    printf("MyLayer Init\n");

    LinceLayer* layer = malloc(sizeof(LinceLayer));
    LINCE_ASSERT(layer, "Failed to allocate layer");
    *layer = (LinceLayer){ 0 };
    layer->OnAttach = MyLayer_OnAttach;
    layer->OnDetach = MyLayer_OnDetach;

    MyLayer* my_layer = malloc(sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");
    my_layer->dummy = 99;

    layer->data.GenericLayer = (void*)my_layer;

    return layer;
}





void GameInit() {
	printf("Game initialised!\n");

    LinceApp_PushLayer(MyLayer_Init());


}

void GameOnUpdate() {

}

void GameOnEvent(Event* e) {

}

void GameTerminate() {
    printf("Game terminated\n");
}


int main(int argc, const char* argv[]) {

    LinceGame_Init_ptr = GameInit;
    LinceGame_OnUpdate_ptr = GameOnUpdate;
    LinceGame_OnEvent_ptr = GameOnEvent;
    LinceGame_Terminate_ptr = GameTerminate;
    
    LinceApp_Run();

    return 0;
}