# Layers

A layer is a set of geometry that is rendered together, usually in a single batch, but it can be split into more batches when there's a lot of vertices.

A layer system in a game engine is important to separate the rendering process into steps, with the vertices on each having different properties, roles, and being rendered in a different order (some layers are rendered after / above others).

## LinceLayer
The data structure that saves the state of a layer.

- `void OnAttach(LinceLayer* layer)`
	- Callback called when a layer is added to the stack.
- `void OnDetach(LinceLayer* layer)`
	- Callback called when a layer is removed from the stack.
- `void OnUpdate(LinceLayer* layer, float dt)`
	- Callback called on every frame.
	- The second argument `float dt` is the delta time in seconds.
- `void OnEvent(LinceLayer* layer, LinceEvent* event)`
	- Callback called when an event is propagated.
- `void* data`
	- Custom user data which should be a pointer to an user-defined layer structure.

### Example code

This is a simple example of how to create and use a layer in Lince.

```c
void MyLayerOnAttach(LinceLayer* layer){
	printf("MyLayer attached\n");
}

void MyLayerOnDetach(LinceLayer* layer){
	printf("MyLayer detached\n");
}

void MyLayerOnUpdate(LinceLayer* layer, float dt){

}

void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e){

}

void GameInit(){

	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = MyLayerOnAttach;
	layer->OnDetach = MyLayerOnDetach;
	layer->OnUpdate = MyLayerOnUpdate;
	layer->OnEvent = MyLayerOnEvent;

	LincePushLayer(layer);
}

int main(){
	// set user callbacks
	LinceApp* app = LinceGetAppState();
	app->game_init = GameInit;
	
	// run the application
	LinceRun();

	return 0;
}
```

## LinceCreateLayer
```c
LinceLayer* LinceCreateLayer(void* data)
```
Allocates a new layer.

Parameters:
- `void* data`
	- Custom user layer data
Returns:
- `LinceLayer*`
	- Pointer to allocated layer

## LinceGetLayerData
```c
void* LinceGetLayerData(LinceLayer* layer)
```
Deprecated. Retrieves user data from a layer.

## LincePushLayer
```c
void LincePushLayer(LinceLayer* layer)
```
Appends a layer to the application's layer stack.

## LincePushOverlay
```c
void LincePushOverlay(LinceLayer* layer)
```
Appends an overlay to the application's overlay stack.

## LinceGetCurrentLayer
```c
LinceLayer* LinceGetCurrentLayer()
```
Retrieves a pointer to the current active layer.
If no layer is active, returns NULL.
This is useful when you need to retrieve layer data from within callbacks called on the layer's OnUpdate and OnEvent functions.

## LinceGetCurrentOverlay
```c
LinceLayer* LinceGetCurrentOverlay()
```
Retrieves a pointer to the current active overlay.
If no overlay is active, returns NULL.
This is useful when you need to retrieve overlat data from within callbacks called on the overlays's OnUpdate and OnEvent functions.


## LinceLayerStack

Stack of layers that are updated from last to first.

- `LinceLayer** layers`
	- Array of layer pointers which constitutes the stack.
- `unsigned int count`
	- Number of layers in the stack.

## LinceCreateLayerStack
```c
LinceLayerStack* LinceCreateLayerStack()
```
Allocates a new layer stack.

## LinceDestroyLayerStack
```c
void LinceDestroyLayerStack(LinceLayerStack*)
```
Deletes all layers on the stack, calling their OnDetach callbacks in order.

## LinceLayerStackPush
```c
void LinceLayerStackPush(LinceLayerStack*, LinceLayer*)
```
Appends a layer to the stack.

## LinceLayerStackPop
```c
void LinceLayerStackPop(LinceLayerStack*, LinceLayer*)
```
Removes a layer from the stack.

