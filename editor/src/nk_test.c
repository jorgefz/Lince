#include "nk_test.h"
#include "lince.h"

static void NKLayerOnUpdate(LinceLayer* layer, float dt){
    
    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    static struct nk_colorf bg = {0.1, 0.18, 0.24, 1.0};
    // uint32_t window_width = LinceGetAppState()->window->width;
    uint32_t window_height = LinceGetAppState()->window->height;

    // LinceUIText(ui, "Text1", 10, 10, LinceFont_Droid30, 10, "FPS %.0f", 1000.0/dt);
    // LinceUIText(ui, "Text2", 10, 40, LinceFont_Droid30, 10, "%.2f ms", dt);
    // LinceUIText(ui, "Title", (float)window_width/2-200, 50, LinceFont_Droid30, 30, "The Legend of Cheesus Christ");

    nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);

        if (nk_button_label(ctx, "button")) { }

        static char buffer[1000] = {0};
        sprintf(buffer, "dt = %.2f ms", dt);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        sprintf(buffer, "fps = %.2f", 1000.0/dt);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
            nk_layout_row_dynamic(ctx, 120, 1);
            bg = nk_color_picker(ctx, bg, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
            bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(ctx);
        }
    } 
    nk_end(ctx);

    LinceSetClearColor(bg.r, bg.g, bg.b, bg.a);
    
    LINCE_UNUSED(window_height);
    LINCE_UNUSED(layer);
}

LinceLayer* NKLayerInit(){
    LinceLayer* layer = LinceCreateLayer(NULL);
    layer->OnUpdate = NKLayerOnUpdate; 
    return layer;
}
