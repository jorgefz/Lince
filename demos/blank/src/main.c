

#include <lince.h>

int main() {

	LinceApp* app = LinceGetApp();
	app->screen_width = 800;
	app->screen_height = 500;
	LinceAppSetTitle("My Window", sizeof("My Window")-1);

	LinceRun();

	return 0;
}