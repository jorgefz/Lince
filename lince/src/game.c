
#include <time.h>

#include "lince.h"
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

void CheckShaderCompilation(int shader, const char* label){
    GLint is_compiled = LinceTrue;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if(!is_compiled)
    {
        size_t length = 1;
        char message[1000] = {0};
        glGetShaderInfoLog(shader, sizeof(message), &length, &message[0]);
        printf("%d bytes returned\n", length);
        printf("Failed to compile shader '%s': %s\n", label, message);
        exit(-1);
    }
}

int CreateShaderFast(){
    int vertex_shader, fragment_shader, program;
    int is_compiled = GL_FALSE;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    printf("VertexShader Glerror %d \n", glGetError());
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) printf("Failed to compile vertex shader!\n");
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    printf("FragmentShader Glerror %d \n", glGetError());
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) printf("Failed to compile vertex shader!\n");
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    printf("Program Glerror %d \n", glGetError());

	glUseProgram(program);
    return program;
}




typedef struct MyLayer {
    float red, vel;
    LinceVertexArray* va;
    LinceBool show_fps;
    LinceBool show_mouse_pos;

    int program, vao, ib;
} MyLayer;


void MyLayer_OnAttach(LinceLayer* layer) {
    printf("MyLayer attached\n");

    MyLayer* data = LinceGetLayerData(layer);
    data->red = 0.0f;
    data->vel = 5e-4f;
    data->show_fps = LinceFalse;
    data->show_mouse_pos = LinceFalse;

    /*
    LinceIndexBuffer ib = LinceCreateIndexBuffer(indices, 6);
    LinceVertexBuffer vb = LinceCreateVertexBuffer(vertices, sizeof(float)*8);
    
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"}
    };
    
    LinceVertexArray* va = LinceCreateVertexArray(ib);
    LINCE_ASSERT(va, "Failed to create vertex array");
    LinceAddVertexArrayAttributes(va, vb, layout, 1);
    data->va = va;
    */

}

void MyLayer_OnDetach(LinceLayer* layer) {
    printf("MyLayer detached\n");
    MyLayer* data = LinceGetLayerData(layer);
    //LinceDeleteVertexArray(data->va);
    free(data);
}


void MyLayer_OnEvent(LinceLayer* layer, LinceEvent* e) {

}

void MyLayer_OnUpdate(LinceLayer* layer, float dt) {
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
    //LinceRender_SetClearColor(data->red, 0.1f, blue, 1.0f);

    /* display fps and mouse position */
    float fps = 1000.0 / dt;
    if (data->show_mouse_pos){ // prints mouse position
        printf(" Mouse: %.2f %.2f ", LinceGetMouseX(), LinceGetMouseY());
        fflush(stdout);
    }
    if (data->show_fps){ // displays and updates frame rate every frame
        printf(" FPS: %.04g (dt: %.04g ms) ", fps, dt);
        fflush(stdout);
    }
    if (data->show_fps || data->show_mouse_pos){
        fflush(stdout);
        for(int i=0; i!=100; ++i) printf("\b");
        fflush(stdout);
    }
}

LinceLayer* MyLayer_Init(int n) {

    MyLayer* my_layer = malloc(sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");
    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayer_OnAttach;
    layer->OnDetach = MyLayer_OnDetach;
    layer->OnEvent = MyLayer_OnEvent;
    layer->OnUpdate = MyLayer_OnUpdate;

    return layer;
}


// =============================================================


void GameInit() {
	printf("Game initialised!\n");
    printf(" Info: press UP to display FPS and DOWN to display mouse position\n");
    printf(" Info: press any alphabetic key to type and backspace to delete\n");
    //LincePushLayer(MyLayer_Init(1));

    static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        -0.5f, -0.5f,   1.0, 0.0, 0.0, 1.0,
         0.5f, -0.5f,   1.0, 1.0, 0.0, 1.0,
         0.5f,  0.5f,   0.0, 1.0, 0.0, 1.0,
        -0.5f,  0.5f,   0.0, 0.0, 1.0, 1.0,
    };

    LinceIndexBuffer ib = LinceCreateIndexBuffer(indices, 6);
    LinceVertexBuffer vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    LinceVertexArray* va = LinceCreateVertexArray(ib);
    LinceBindVertexArray(va);
    LinceBindIndexBuffer(ib);
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float4, "aColor"},
    };
    LinceAddVertexArrayAttributes(va, vb, layout, 2);

    // Shader
    CreateShaderFast();
}


void GameOnUpdate(float dt) {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
