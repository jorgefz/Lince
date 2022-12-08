#include "lince.h"
#include "pong.h"


void InitLayers() {
	LINCE_INFO("\n Pong Initialised");
    LincePushLayer(GameLayerInit());
}

void OnUpdate(float dt) {
    LinceCheckErrors();
    LINCE_UNUSED(dt);
}


void SetupAppWindow(){
    LinceApp* app = LinceGetAppState();

    app->screen_width = 900;
    app->screen_height = 600;
    app->title = "Pong";

    app->game_init = InitLayers;
    app->game_on_update = OnUpdate;
}


int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    SetupAppWindow();
    
    LinceRun();

    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}