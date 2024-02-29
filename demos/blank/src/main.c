

#include <lince.h>

int main() {

	LinceApp* app = LinceGetApp();
	app->screen_width = 800;
	app->screen_height = 500;
	LinceAppSetTitle("My Window");

	LinceRun();

	return 0;
}