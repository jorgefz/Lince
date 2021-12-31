
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lince.h"

typedef struct {
    LinceWindow* window;
} LinceApp;

LinceApp app = {0};

void LinceApp_OnEvent(Event* e);

void LinceApp_Init(){
    // Create a windowed mode window and its OpenGL context
    app.window = LinceWindow_Create(1280, 720);
    LinceWindow_SetEventCallback(app.window, LinceApp_OnEvent);
}

void LinceApp_OnUpdate(){
    glClear(GL_COLOR_BUFFER_BIT);
    LinceWindow_Update(app.window);
}

unsigned int LinceApp_OnKeyPressed(Event* e){
    printf("Key %d\n", e->data.KeyPressed->keycode);
    return 0;
}

unsigned int LinceApp_OnMouseMoved(Event* e){
    printf("Mouse %f %f\n", e->data.MouseMoved->xpos, e->data.MouseMoved->ypos);
    return 0;
}

void LinceApp_OnEvent(Event* e){
    LinceEvent_Dispatch(e, EventType_MouseMoved, LinceApp_OnMouseMoved);
    LinceEvent_Dispatch(e, EventType_KeyPressed, LinceApp_OnKeyPressed);
}

void LinceApp_Run(){

    float r = 0.0f, v = 0.0001f;
    while (!LinceWindow_ShouldClose(app.window)) {
        // Render here
        r += v;
        if (r >= 1.0f) v = -v;
        else if (r <= 0.0f) v = -v;
        
        glClearColor(r, 0.1f, 0.6f, 1.0f);
        LinceApp_OnUpdate();
    }
}

void LinceApp_Terminate(){
    LinceWindow_Destroy(app.window);
    app.window = NULL;
}


int main(int argc, const char* argv[]){

    LinceApp_Init();
    LinceApp_Run();
    LinceApp_Terminate();
    
    return 0;
}


// User-defined function pointers

void (*LinceGame_Init_ptr)(LinceWindow*, void* args);

void (*LinceGame_OnUpdate_ptr)(LinceWindow*, void* args);

void (*LinceGame_OnEvent_ptr)(LinceWindow*, void *args);

void (*LinceGame_Terminate_ptr)(LinceWindow*, void *args);

