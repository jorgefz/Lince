
#include "editor.h"
#include <time.h>
#include <lince.h>


void SetupApplication(){
    LinceApp* app = LinceGetApp();

    // app->user_data = NULL;
    app->screen_width = 1280;
    app->screen_height = 720;
    app->title = "Sandbox";
    // app->flags = LINCE_FULLSCREEN | LINCE_VSYNC | LINCE_RESIZEABLE | ...

    app->on_init = EditorInit;
    app->on_update = EditorOnUpdate;
    app->on_event = EditorOnEvent;
    app->on_terminate = EditorTerminate;
}


int main(int argc, const char* argv[]) {
    srand(time(NULL));

    SetupApplication();
    LinceRun();
    
    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}
