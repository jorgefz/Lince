#include <lince.h>


int main(){
    LinceApp* app = LinceGetApp();
    app->show_debug_panel = LinceTrue;
    
    LinceRun();
    return 0;
}