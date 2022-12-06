#include "lince.h"
#include "pong.h"

void GameInit() {
	LINCE_INFO("\n Pong Initialised");
    LincePushLayer(PongLayerInit());
}

void GameOnUpdate(float dt) {
    LinceCheckErrors();
    LINCE_UNUSED(dt);
}

void GameTerminate() {
    LINCE_INFO(" Pong Terminated");
}

int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    LinceApp* app = LinceGetAppState();

    app->screen_width = 900;
    app->screen_height = 600;
    app->title = "Sandbox";

    app->game_init = GameInit;
    app->game_on_update = GameOnUpdate;
    app->game_terminate = GameTerminate;
    
    LinceRun();

    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}