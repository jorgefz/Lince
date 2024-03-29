# User guide

## Create a window

Once you have setup the config files and linked Lince to your project, it is time to include it:
```c
#include <lince.h>

int main(){
    return 0;
}
```

To run the application with an empty window, simply call `LinceRun`.
```c
#include <lince.h>

int main(){
    LinceRun();
    return 0;
}
```

## Customise your window

To change the initial settings of your window, you can
retrieve and modify its initial state with `LinceGetApp` before running `LinceRun`.

Set its initial width and height in pixels with `app->width` and `app->height`, and the title at the top bar with `app->title`.

```c
#include <lince.h>

int main(){
    LinceApp* app = LinceGetApp();
    app->width = 1920;
    app->height = 1080;
    app->title = "My window";

    LinceRun();
    return 0;
}
```


## Update your window

To start working on your game and drawing stuff to the screen,
you should provide the application with a callback function (such as `OnUpdate`
in the example below) that will be called on every frame.
This callback will receive the frame time `dt`, which is the number of
milliseconds that have elapsed since the last frame.

```c
#include <lince.h>

void OnUpdate(float dt){
    printf("Frame time: %f ms", dt);
}

int main(){
    LinceApp* app = LinceGetApp();
    app->width = 1920;
    app->height = 1080;
    app->title = "My window";

    app->on_update = OnUpdate;

    LinceRun();
    return 0;
}
```

## Init and uninit

```c
#include <lince.h>

void OnInit(){
    printf("Application started!\n");
}

void OnUpdate(float dt){
    printf("Frame time: %f ms", dt);
}

void OnTerminate(){
    printf("Application closed!\n");
}

int main(){
    LinceApp* app = LinceGetApp();
    app->width = 1920;
    app->height = 1080;
    app->title = "My window";

    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_terminate = OnTerminate;

    LinceRun();
    return 0;
}
```


## Camera

```c
#include <lince.h>

LinceCamera* camera;

void OnInit(){
    camera = LinceCreateCamera(LinceGetAspectRatio());
}

void OnUpdate(float dt){
    LinceResizeCameraView(camera, LinceGetAspectRatio());
	LinceUpdateCamera(camera);
    // printf("Frame time: %f ms", dt);
}

void OnTerminate(){
    LinceDeleteCamera(camera);
}

int main(){
    LinceApp* app = LinceGetApp();
    app->width = 1920;
    app->height = 1080;
    app->title = "My window";

    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_terminate = OnTerminate;

    LinceRun();
    return 0;
}
```

## DrawSprite

## Customise sprite
## User input
## Move camera
## Events
## Textures
## Shaders
## ECS
## Tilemaps
## Tile animations
## Colliders
## Sounds
## GUI
