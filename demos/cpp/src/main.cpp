#include <lince.h>


int main(){
    LinceApp* app = LinceGetApp();
    app->show_debug_panel = LinceTrue;

    LinceAppSetConfigFile(string_scoped_lit("demos/cpp/project.toml"));
    
    LinceRun();
    return 0;
}