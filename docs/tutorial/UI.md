# UI

The Lince engine uses Nuklear for its graphical user interface. See [the gihub repo](https://github.com/Immediate-Mode-UI/Nuklear/) for detailed documentation. This document will compile useful Nuklear functions as well a Lince wrappers.

Any of Nuklear's window functions (anything between and including `nk_begin` and `nk_end`) can be used on the `OnUpdate` callbacks of any given layer. See [Layers.md](./Layers.md) for a descriptopn on how to use layers in Lince. 

Example:
```c
void MyLayerOnUpdate(LinceLayer* layer, float dt){
	struct nk_context *ctx = LinceGetApp()->ui->ctx;

	if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
		nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button")){
			printf("button pressed\n");
		}
	}
	nk_end(ctx);
}
```

## LinceUI
Wrapper that saves Nuklear's context and its GLFW rendering state.

- `struct nk_context *ctx`
- `void *glfw`
- `void *glfw_window`
- `struct nk_font* fonts[LinceFont_Count]`


## LinceFonts

Enum that offers a handy way to obtain a preloaded Nuklear font.
The values serve as an index for the array `app->ui->fonts` which holds pointers to all preloaded fonts.
The number next to the enum name indicates the font size, e.g. `LinceFont_Droid20` indexes the Droid font with font size 20.

|       Enum        | Value |
| ----------------- | ----- |
| LinceFont_Droid15 |   0   |
| LinceFont_Droid20 |   1   | 
| LinceFont_Droid30 |   2   | 
| LinceFont_Droid50 |   3   | 
| LinceFont_Count   |   4   |

Example on how to retrieve a preloaded font:
```c
void MyLayerOnUpdate(LinceLayer* layer, float dt){
	LinceApp* app = LinceGetApp();
	struct nk_font* title_font = app->ui->fonts[LinceFont_Droid50];
	nk_style_set_font(app->ui->ctx, &title_font->handle);
}
```


## LinceUIText
```c
void LinceUIText(
    LinceUI* ui,
    const char* name,
    float x,
	float y,
    LinceFonts font, 
    size_t max_size, 
    const char* text,
    ...
)
```
Wrapper function that renders plain text to the screen.
Parameters:
- `LinceUI* ui`
	- UI state, this can be retrieved from the application state `LinceGetApp()->ui`.
- `const char* name`
	- Unique string identifier for the Nuklear window.
- `float x`
	- X position of the text in pixels, measured from the left-end of the text box.
- `float y`
	- Y position of the text in pixels, measred from the top of the text box.
- `LinceFonts font`
	- Font and font size to use. See below for available fonts.
- `size_t max_size`
	- Maximum number of characters to display.
- `const char* text, ...`
	- String to render. This may be formatted using `printf` conventions and additional variadic arguments.

This example renders the current delta-time and FPS to the upper left corner of the screen:
```c
void MyLayerOnUpdate(LinceLayer* layer, float dt){

	LinceUI* ui = LinceGetApp()->ui;

	/*          ui,  identifier,     x,  y,  font,              max, text,       format varargs */
	LinceUIText(ui, "FPS_indicator", 10, 10, LinceFont_Droid30, 10,  "FPS %.0f", 1000.0/dt);
    LinceUIText(ui, "DT_indicator",  10, 40, LinceFont_Droid30, 10,  "%.2f ms",  dt);
}
``` 


