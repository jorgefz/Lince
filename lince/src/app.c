
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "app.h"

LinceApp app = {0};

LinceApp* LinceApp_GetApplication(){
    return &app;
}

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

unsigned int LinceApp_OnWindowResize(Event* e){
    printf("Window resized to %d x %d\n", e->data.WindowResize->width, e->data.WindowResize->width);
    return 0;
}

void LinceApp_OnEvent(Event* e){
    //LinceEvent_Dispatch(e, EventType_MouseMoved, LinceApp_OnMouseMoved);
    LinceEvent_Dispatch(e, EventType_KeyPressed, LinceApp_OnKeyPressed);
    LinceEvent_Dispatch(e, EventType_WindowResize, LinceApp_OnWindowResize);
}

void LinceApp_Run(){

    float r = 0.0f, v = 0.0001f, b;
    while (!LinceWindow_ShouldClose(app.window)) {
        // Render here
        r += v;
        if (r >= 1.0f) v = -v;
        else if (r <= 0.0f) v = -v;
        b = 1.0f - r;        
        glClearColor(r, 0.1f, b, 1.0f);
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


