/*

Controls
- Camera movement:
    Left click and drag
    WASD keys
- Entity interact
    Control and left click to select
    Right click and drag to move ??

*/



#include "editor.h"
#include "platform/platform.h"
#include <math.h>
#include "nuklear_flags.h"
#include <lince.h>

#define TAG_MAX 100
typedef struct LinceTag {
    char tag[TAG_MAX];
} LinceTag;




/*
static const char* COMPNAMES[] = {
    "Tag",
    "BoxCollider",
    "Sprite",
    "Shader"
};
*/

typedef struct EditorState {

    string_t fpath;   // Full path of loaded tilemap.
    LinceBool loaded; // Whether a tilemap is loaded.
    LinceBool saved;  // Whether the current tilemap is saved.
    LinceTilemap* tmap; // Tilemap being edited

    LinceTileset* tset; // Tileset object
    string_t tset_name; // String ID of tileset 

    LinceCamera* camera;
    LinceBool mouse_drag;
    vec2 mouse_drag_from;
} EditorState;
EditorState STATE = {0};


void nk_edit_float(struct nk_context* ctx, float* value){
    if(!value) return;
    static char buffer[50] = {0};
    snprintf(buffer, sizeof(buffer)-1, "%.7f", *value);
    nk_edit_string_zero_terminated(
        ctx, NK_EDIT_FIELD,
        buffer, sizeof(buffer)-1, nk_filter_float
    );
    *value = (float)atof(buffer);
}


/*
void SpriteGUI(struct nk_context* ctx, uint32_t entity_id){
    LinceSprite* sprite = LinceGetEntityComponent(STATE.reg, entity_id, Component_Sprite);
    if(!sprite) return;

    static int tree_id = 100;
    if (nk_tree_push_id(ctx, NK_TREE_TAB, "Sprite", NK_MINIMIZED, tree_id)) {
        nk_layout_row_dynamic(ctx, 30, 2);
        // nk_property_float(ctx, name, float min, float *val, float max, float step, float inc_per_pixel);
        nk_property_float(ctx, "#x", -10.0, &sprite->x, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "#y", -10.0, &sprite->y, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "#w",   0.0, &sprite->w, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "#h",   0.0, &sprite->h, 10.0, 0.1, 0.1);

        // Color picker
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Color", NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 100, 1);
        struct nk_colorf color;
        memmove(&color, &sprite->color, sizeof(float)*4);
        nk_color_pick(ctx, &color, NK_RGBA);
        memmove(&sprite->color, &color, sizeof(float)*4);

        // Color values
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_labelf(ctx, NK_TEXT_CENTERED,
            "%.2f %.2f %.2f %.2f",
            sprite->color[0], sprite->color[1],
            sprite->color[2], sprite->color[3]
        );

        static char buffer[50] = {0};
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD,
            buffer, 49, nk_filter_float);

        nk_labelf(ctx, NK_TEXT_CENTERED, "Value: %.3f", (float)atof(buffer));

        static float value = 0.0f;
        nk_edit_float(ctx, &value);

        nk_tree_pop(ctx);
    }
}
*/


LinceBool DrawGUINewTilemapHeightWidthBox(LincePoint* new_dims){
    struct nk_context* ctx = LinceUIGetNkContext(LinceGetApp()->ui);
    LincePoint screen = LinceAppGetWindowSize();
    
    // State
    static int height = 100, width = 100;
    static LinceBool finish = LinceFalse;

    // Draw window
    if (nk_begin(ctx, "New Tilemap", nk_rect(screen.x/2, screen.y/2, 300, 200), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE | NK_WINDOW_SCALABLE)){
        nk_layout_row_static(ctx, 30, 175, 1);
        height = nk_propertyi(ctx, "#Height", 1, height, 1000, 1, 1);
        width  = nk_propertyi(ctx, "#Width",  1, width, 1000, 1, 1);
        if(nk_button_label(ctx, "OK")){
            finish = LinceTrue;
        }
        nk_end(ctx);
    }

    // Check / reset state
    if(finish){
        // Height and width chosen.
        // Reset state and return dims.
        finish = LinceFalse;
        new_dims->x = (float)height;
        new_dims->y = (float)width;
        height = 100;
        width  = 100;
        return LinceTrue;
    }

    return LinceFalse;
}

void DrawGUITopBar(){
    struct nk_context* ctx = LinceUIGetNkContext(LinceGetApp()->ui);
    LincePoint screen = LinceAppGetWindowSize();

    const struct nk_style_button topbar_button_style = {
        /* background */
        .normal = nk_style_item_color((struct nk_color){ 45, 45, 45,255}), // #323232
        .hover  = nk_style_item_color((struct nk_color){100,100,100,255}), // #646464
        .active = nk_style_item_color((struct nk_color){100,100,100,255}), // #646464
        /* text */
        .text_background = {0,0,0,255},
        .text_normal     = {255,255,255,255},
        .text_hover      = {255,255,255,255},
        .text_active     = {255,255,255,255},
        .text_alignment  = NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_CENTERED,
        .padding = {0,0}
    };

    float topbar_h = 35; // SHOULD BE GLOBAL

    static LinceBool do_new_tilemap = LinceFalse;
    LincePoint new_tilemap_dims = {0};

    nk_style_push_style_item(ctx, &ctx->style.menu_button.hover, topbar_button_style.hover);
    nk_style_push_vec2(ctx, &ctx->style.menu_button.padding, topbar_button_style.padding);

    if (nk_begin(ctx, "TopBar", nk_rect(0,0, screen.x, topbar_h), NK_WINDOW_BORDER)){
        nk_menubar_begin(ctx);

        nk_layout_row_begin(ctx, NK_STATIC, topbar_h, 4);
        nk_layout_row_push(ctx, topbar_h);
        // nk_layout_row_begin(ctx, NK_STATIC, 25, 1);
        // nk_layout_row_push(ctx, 45);

        /* ================== FILE ================== */
        if(nk_menu_begin_label(ctx, "File", NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_CENTERED, nk_vec2(200, 200))){
            nk_layout_row_dynamic(ctx, 25, 1);

            /* ===== CREATE NEW TILEMAP ===== */
            if(nk_menu_item_label(ctx, "New",  NK_TEXT_LEFT)){
                do_new_tilemap = LinceTrue;
                if (STATE.loaded && !STATE.saved){
                    do_new_tilemap = LinceEditorShowMessageBoxYesNo("Error", "This will delete any unsaved changes. Continue?");
                }
            }

            /* ===== LOAD NEW TILEMAP ===== */
            if(nk_menu_item_label(ctx, "Load", NK_TEXT_LEFT)){
                LinceBool load_new = LinceTrue;
                if(STATE.loaded && !STATE.saved){
                    load_new = LinceEditorShowMessageBoxYesNo("Error", "This will delete any unsaved changes. Continue?");
                }
                if (load_new){
                    string_t new_fpath = LinceEditorOpenLoadFileDialog();
                    if(string_ok(new_fpath)){
                        printf("%s\n", new_fpath.str);
                        LinceTilemap* new_tmap = LinceLoadTilemapAsset(LinceAppGetAssetCache(), new_fpath, NULL);

                        if (new_tmap){
                            if(string_ok(STATE.fpath)) string_free(&STATE.fpath);
                            if(STATE.tmap) LinceUnloadTilemapAsset(LinceAppGetAssetCache(), STATE.tmap);
                            STATE.fpath = new_fpath;
                            STATE.tmap = new_tmap;
                            STATE.loaded = LinceTrue;
                            STATE.saved  = LinceTrue;
                        } else {
                            LinceEditorShowMessageBox("Error", "Could not load tilemap from file");
                            string_free(&new_fpath);
                        }
                    }
                }
                
            }

            /* ===== LOAD NEW TILESET ===== */
            if(nk_menu_item_label(ctx, "Load Tileset", NK_TEXT_LEFT)){
                string_t new_tset_path = LinceEditorOpenLoadFileDialog();
                if(string_ok(new_tset_path)){
                    LinceTileset* new_tset = LinceLoadTilesetAsset(LinceAppGetAssetCache(), new_tset_path, NULL);
                    if(new_tset){
                        if (STATE.tset) LinceUnloadTilesetAsset(LinceAppGetAssetCache(), STATE.tset);
                        if (!STATE.tmap->tileset){
                            LinceTilemapUseTileset(STATE.tmap, new_tset);
                        }
                        STATE.tset = new_tset;
                    } else {
                        LinceEditorShowMessageBox("Error", "Could not load tileset from file");
                        string_free(&new_tset_path);
                    }
                }
            }

            if(nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT)){
                if(STATE.tmap && string_ok(STATE.fpath)
                EditorSaveTilemap(STATE.tmap, STATE.fpath, STATE.tset_name);
            }
            if(nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT)){  }
            
            nk_menu_end(ctx);
        }

        /* ================== EDIT ================== */
        if(nk_menu_begin_label(ctx, "Edit", NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_CENTERED, nk_vec2(200, 200))){
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "Load", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "Save As", NK_TEXT_LEFT);
            nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT);
            nk_menu_end(ctx);
        }

        nk_menubar_end(ctx);
        nk_end(ctx);
    }

    if(do_new_tilemap && DrawGUINewTilemapHeightWidthBox(&new_tilemap_dims)){
        // Create new tilemap
        do_new_tilemap = LinceFalse;
    }

    nk_style_pop_style_item(ctx);
    nk_style_pop_vec2(ctx);
}

void DrawGUISidePanel(){
    struct nk_context* ctx = LinceUIGetNkContext(LinceGetApp()->ui);
    LincePoint screen = LinceAppGetWindowSize();
    
    float topbar_h = 35; // SHOULD BE GLOBAL
    float sidebar_w = 250; // SHOULD BE GLOBAL
    float sidebar_h = screen.y - topbar_h;
    enum nk_flags sidebar_flags = NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE;

    if (nk_begin(ctx, "SideBar", nk_rect(0,topbar_h, sidebar_w, sidebar_h), sidebar_flags)){
        nk_layout_row_dynamic(ctx, topbar_h, 1);

        nk_label(ctx, "--- TILEMAP ---", 0);
        if(STATE.tmap){
            nk_label(ctx, STATE.fpath.str, NK_TEXT_ALIGN_LEFT);
            nk_labelf(ctx, 0, "Width: %u", STATE.tmap->width);
            nk_labelf(ctx, 0, "Height: %u", STATE.tmap->height);
        } else {
            nk_label(ctx, "No tilemap loaded", NK_TEXT_ALIGN_LEFT);
        }

        nk_label(ctx, "--- TILESET ---", NK_TEXT_ALIGN_LEFT);
        if(string_ok(STATE.tset_name)){
            nk_label(ctx, STATE.tset_name.str, NK_TEXT_ALIGN_LEFT);
        } else {
            nk_label(ctx, "No tileset loaded", NK_TEXT_ALIGN_LEFT);
        }

        // nk_button_symbol();
        // NK_SYMBOL_PLUS
        // NK_SYMBOL_MINUS
        
        
        // Add tilemap button
        /*
        if (nk_button_label(ctx, "New Entity")){
            uint32_t id = LinceCreateEntity(STATE.reg);
            // Add tag as a display name
            LinceTag tag = {0};
            int n = snprintf(tag.tag, TAG_MAX, "Entity %u", id);
            LINCE_ASSERT(n > 0, "Failed to format entity tag string");
            LinceAddEntityComponent(STATE.reg, id, Component_Tag, &tag);
            // Add collision box
            LinceBoxCollider box = {0};
            LinceAddEntityComponent(STATE.reg, id, Component_BoxCollider, &box);
            // Add sprite
            LinceSprite sprite = {.w=1.0, .h=1.0, .color={1,1,1,1}};
            LinceAddEntityComponent(STATE.reg, id, Component_Sprite, &sprite);
        }
        */

    }
    nk_end(ctx);
}

void DrawGUI(){

    struct nk_context* ctx = LinceUIGetNkContext(LinceGetApp()->ui);
    nk_style_push_font(ctx, LinceUIGetFontHandle(LinceGetApp()->ui, string_scoped_lit("droid20")));
    // LinceUIUseFont(LinceGetApp()->ui, string_scoped_lit("droid20"));
    
    DrawGUITopBar();
    DrawGUISidePanel();

    nk_style_pop_font(ctx);
}


void MoveCamera(float dt){
    static const float camera_speed = 1e-3; // units/frame
    const float ds = camera_speed * dt * STATE.camera->zoom;
    if(LinceIsKeyPressed(LinceKey_w)) STATE.camera->pos[1] += ds;
    if(LinceIsKeyPressed(LinceKey_s)) STATE.camera->pos[1] -= ds;
    if(LinceIsKeyPressed(LinceKey_d)) STATE.camera->pos[0] += ds;
    if(LinceIsKeyPressed(LinceKey_a)) STATE.camera->pos[0] -= ds;

    /* ===== MOUSE DRAG ===== */
    if(LinceIsMouseButtonPressed(LinceMouseButton_Left)
        // && LinceIsKeyPressed(LinceKey_LeftControl)
    ){
        LincePoint mouse_pos = LinceGetMousePosWorld(STATE.camera);
        if(!STATE.mouse_drag){
            STATE.mouse_drag_from[0] = mouse_pos.x;
            STATE.mouse_drag_from[1] = mouse_pos.y;
        } else {
            vec2 dr = { mouse_pos.x - STATE.mouse_drag_from[0],
                        mouse_pos.y - STATE.mouse_drag_from[1] };
            STATE.camera->pos[0] -= dr[0] * 1.5f;
            STATE.camera->pos[1] -= dr[1] * 1.5f;
        }
        STATE.mouse_drag = LinceTrue;

    } else {
        STATE.mouse_drag = LinceFalse;
    }
}


void EditorSaveTilemap(LinceTilemap* tm, string_t fpath, string_t tset){
    FILE* f = fopen(fpath.str, "w");
    
    fprintf(f, "mapwidth    = %u \n", tm->width);
    fprintf(f, "mapheight   = %u \n", tm->height);
    fprintf(f, "centerx     = %f \n", tm->pos.x);
    fprintf(f, "centery     = %f \n", tm->pos.y);
    fprintf(f, "scalewidth  = %f \n", tm->scale.x);
    fprintf(f, "scaleheight = %f \n", tm->scale.y);
    fprintf(f, "tileset     = \"%s\" \n", tset.str);

    fprintf(f, "grid        = [\n");
    for(uint32_t i = 0; i != tm->height; ++i){
        fprintf(f, "    ");
        for(uint32_t j = 0; j != tm->width; ++j){
            fprintf(f, "%3u,", *(uint32_t*)array_get(&tm->indices, i * tm->height + j));
        }
        fprintf(f, "\n");
    }
    fprintf(f, "]\n");
    fclose(f);
}


void EditorInit(){
    STATE.camera = LinceCreateCamera(LinceAppGetAspectRatio());
    LinceAppPushAssetFolder(string_scoped_lit("../../../sandbox/assets"));
}

void EditorOnUpdate(float dt){
    MoveCamera(dt);
    LinceUpdateCameraProjection(STATE.camera, LinceAppGetAspectRatio());
	LinceUpdateCamera(STATE.camera);
    
    DrawGUI();

    if (STATE.tmap){
        LinceBeginRender(STATE.camera);
        LinceDrawTilemap(STATE.tmap, NULL);
        LinceEndRender();
    }
}

void EditorOnEvent(LinceEvent* event){

    if(event->type == LinceEventType_MouseScroll){
        LinceMouseScrollEvent* scroll = event->data.mouse_scroll;    
        STATE.camera->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
        return;

    } else if (event->type == LinceEventType_KeyRelease){
        LinceKeyReleaseEvent* ev = event->data.key_release;

        /* ===== SAVE TILEMAP ===== */
        if(ev->keycode == LinceKey_s && (ev->mods & LinceKeyMod_Control)){
            if(STATE.tmap && string_ok(STATE.tset_name)){
                if(string_ok(STATE.fpath)){ 
                    // Save tilemap to file
                    EditorSaveTilemap(STATE.tmap, STATE.fpath, STATE.tset_name);
                } else {
                    // If no fpath specified, open dialogbox and provide path.
                    string_t new_fpath = LinceEditorOpenSaveFileDialog();
                    if(string_ok(new_fpath)){
                        STATE.fpath = new_fpath;
                        EditorSaveTilemap(STATE.tmap, STATE.fpath, STATE.tset_name);
                    }
                }
            }
        }
    }
}


void EditorTerminate(){
    if(string_ok(STATE.fpath)){
        string_free(&STATE.fpath);
    }

    if(STATE.tmap){
        LinceTilemapUninit(STATE.tmap);
    }
    
}