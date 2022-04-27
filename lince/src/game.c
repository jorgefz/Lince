
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
        int length = 1;
        char message[1000] = {0};
        glGetShaderInfoLog(shader, sizeof(message), &length, &message[0]);
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
    printf("# VertexShader Glerror %d \n", glGetError());
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) printf("Failed to compile vertex shader!\n");
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    printf("# FragmentShader Glerror %d \n", glGetError());
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) printf("Failed to compile vertex shader!\n");
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    printf("# Program Glerror %d \n", glGetError());

	glUseProgram(program);
    return program;
}




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

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        LINCE_ASSERT(LinceFalse, "GLerror: %d", err);
    }

    /* update background color */
    data->red += data->vel * dt;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;
    LinceRender_SetClearColor(data->red, 0.1f, blue, 1.0f);

}

LinceLayer* MyLayerInit(char* name) {

    MyLayer* my_layer = calloc(1, sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len = strlen(name) < LINCE_STR_MAX ? strlen(name) : LINCE_STR_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayerOnAttach;
    layer->OnDetach = MyLayerOnDetach;
    layer->OnEvent = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================

LinceVertexArray *global_va;

void GameInit() {
	LINCE_INFO("\n User App Initialised");

    LincePushLayer(MyLayerInit("Test"));

    static unsigned int indices[] = {0,1,2,2,3,0};
    static float vertices[] = {
        -0.5f, -0.5f,   1.0, 0.0, 0.0, 1.0,
         0.5f, -0.5f,   1.0, 1.0, 0.0, 1.0,
         0.5f,  0.5f,   0.0, 1.0, 0.0, 1.0,
        -0.5f,  0.5f,   0.0, 0.0, 1.0, 1.0,
    };

    LinceIndexBuffer ib = LinceCreateIndexBuffer(indices, 6);
    LinceVertexBuffer vb = LinceCreateVertexBuffer(vertices, sizeof(vertices));
    global_va = LinceCreateVertexArray(ib);

    LinceBindVertexArray(global_va);
    LinceBindIndexBuffer(ib);
    LinceBufferElement layout[] = {
        {LinceBufferType_Float2, "aPos"},
        {LinceBufferType_Float4, "aColor"},
    };
    unsigned int elems = sizeof(layout) / sizeof(LinceBufferElement);
    LinceAddVertexArrayAttributes(global_va, vb, layout, elems);

    // Shader
    CreateShaderFast();
}


void GameOnUpdate(float dt) {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void GameOnEvent(LinceEvent* e) {

}


void GameTerminate() {
    LinceDeleteVertexArray(global_va);
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
