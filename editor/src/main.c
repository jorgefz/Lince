
#include "editor.h"
#include <time.h>
#include <lince.h>


void SetupApplication(){
    LinceApp* app = LinceGetAppState();

    // app->user_data = NULL;
    app->screen_width = 1280;
    app->screen_height = 720;
    app->title = "Sandbox";
    // app->flags = LINCE_FULLSCREEN | LINCE_VSYNC | LINCE_RESIZEABLE | ...

    app->game_init = EditorInit;
    app->game_on_update = EditorOnUpdate;
    app->game_on_event = EditorOnEvent;
    app->game_terminate = EditorTerminate;
    
    app->enable_profiling = LinceTrue;
    app->profiler_filename = "tests/profiling/profile.txt";
}


int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    srand(time(NULL));

    SetupApplication();
    
    LinceRun();
    
    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}
