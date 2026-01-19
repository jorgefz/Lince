#include <lince.h>

#include <nuklear_flags.h>
#include <nuklear.h>

#include <nfd.h>

#define TMAPEDIT_VERSION "1.0"


typedef struct tme_state {
    LinceCamera* camera;
    LinceBool mouse_drag;
    float mouse_drag_from[2];
    LincePoint wsize;

    LinceTilemap* tmap;
    LinceTileset* tset;
    LinceTexture* tset_tex;

    string_t tmap_path;
    string_t tset_path;
    string_t tset_name;
    int      tilesize; // Size in pixels of a tile

    LinceBool tset_loaded;
    LinceBool tmap_loaded;
    LinceBool tmap_saved;
    int draw_tset_tile; // Index of chosen tile in tileset chosen for drawing
    int edit_tmap_tile; // Index of tilemap tile being hovered, -1 if none.

    // GUI STATE
    LinceBool show_new_tmap_menu;
    float grid_thickness;
    LinceBool show_grid;

} tme_state;

static tme_state STATE = {0};


/* Flips the texture coordinates of each tile in a tileset on the Y axis.
 * This is done because Nuklear and Lince read textures in flipped directions on the Y axis:
 * For Nuklear, the origin is in the upper left corner, whilst for Lince this is on the lower left corner.
 */
void tme_flip_tileset_coords(LinceTileset* tset){
	uint32_t xcells = tset->xcells;
	uint32_t ycells = tset->ycells;

    for (uint32_t x = 0; x != xcells; ++x){
		for (uint32_t y = 0; y < ycells/2; ++y){
            LinceRect temp;
            LinceRect *r1 = array_get(&tset->coords, xcells * y + x);
            LinceRect *r2 = array_get(&tset->coords, xcells * (ycells - y - 1) + x);
            temp = *r1;
            *r1 = *r2;
            *r2 = temp;
		}
	}
}


LinceBool tme_write_tilemap(LinceTilemap* map, string_t path, string_t tset_name){

    if(!map || !string_ok(path) || !string_ok(tset_name)) return LinceFalse;

    FILE* f = fopen(path.str, "w");

    fprintf(f, "mapwidth    = %u\n", map->width);
    fprintf(f, "mapheight   = %u\n", map->height);
    fprintf(f, "centerx     = %f\n", map->pos.x);
    fprintf(f, "centery     = %f\n", map->pos.y);
    fprintf(f, "scalewidth  = %f\n", map->scale.x);
    fprintf(f, "scaleheight = %f\n", map->scale.y);
    fprintf(f, "tileset     = \"%s\"\n", tset_name.str);

    fprintf(f, "grid        = [");
    for(uint32_t* idx = map->indices.begin, i = 0; idx != map->indices.end; ++idx, ++i){
        if((i % map->width) == 0){
            fprintf(f, "\n    ");
        }
        fprintf(f, "%3u,", *idx);
    }
    fprintf(f,"\n]");
    fclose(f);

    printf("[TMAPEDIT] Tilemap written to '%s'\n", path.str);

    return LinceTrue;
}


LinceBool tme_load_tilemap(LinceTilemap* map, string_t path, string_t *tset_name){
    if(!string_ok(path) || !map || !tset_name) return LinceFalse;

    string_t content = LinceReadFile(path);
	if(!string_ok(content)){
		LINCE_ERROR("Could not read tilemap at '%s'", path.str);
		return LinceFalse;
	}

    char errbuf[256];
	toml_table_t* tab = toml_parse(content.str, errbuf, sizeof(errbuf));
    string_free(&content);

    if(!tab){
		LINCE_ERROR("Error parsing TOML file '%s'", path.str);
		LINCE_ERROR(" -> %s", errbuf);
	    return LinceFalse;
    }

	toml_datum_t w     = toml_int_in(tab, "mapwidth");
	toml_datum_t h     = toml_int_in(tab, "mapheight");
	toml_datum_t cx    = toml_double_in(tab, "centerx");
    toml_datum_t cy    = toml_double_in(tab, "centery");
	toml_datum_t sw    = toml_double_in(tab, "scalewidth");
    toml_datum_t sh    = toml_double_in(tab, "scaleheight");
    toml_datum_t tset  = toml_string_in(tab, "tileset");
    toml_array_t* grid = toml_array_in(tab, "grid");

    if(!w.ok || !h.ok || !grid){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Invalid parameters: %s%s%s",
            w.ok ? "" : "mapwidth, ",
            h.ok ? "" : "mapheight, ",
            grid ? "" : "grid"
        );
        toml_free(tab);
        return LinceFalse;
    }

    uint32_t width  = (uint32_t)w.u.i;
    uint32_t height = (uint32_t)h.u.i;
    uint32_t nelem  = (uint32_t)toml_array_nelem(grid);
    if(nelem != width*height){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Expected %lu items but grid has %lu", width*height, nelem);
        toml_free(tab);
        return LinceFalse;
    } else if ( (toml_array_kind(grid) != 'v') || (toml_array_type(grid) != 'i') ){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Grid values should be integers");
        toml_free(tab);
        return LinceFalse;
    }

    uint32_t* grid_data = LinceAlloc(sizeof(uint32_t) * nelem);
    for(uint32_t i = 0; i != nelem; ++i){
        grid_data[i] = (uint32_t)toml_int_at(grid, i).u.i;
    }

    LincePoint pos, scale;
    pos.x   = cx.ok ? (float)cx.u.d : 0.0f;
    pos.y   = cy.ok ? (float)cy.u.d : 0.0f;
    scale.x = sw.ok ? (float)sw.u.d : 1.0f;
    scale.y = sh.ok ? (float)sh.u.d : 1.0f;
    
    map->width  = width;
    map->height = height;
    map->pos    = pos;
    map->scale  = scale;

    void *success = LinceTilemapInit(map, grid_data);
    if(!success){
        if(tset.ok) LinceFree(tset.u.s);
        LinceFree(grid_data);
        toml_free(tab);
    }

    if(tset.ok){
        *tset_name = string_from_chars(tset.u.s, strlen(tset.u.s));
    }

    return LinceTrue;
}

void tme_draw_tilemap(LinceTilemap* map, LinceCamera* cam){
    if(!map->tileset) return;

    for(uint32_t x = 0; x != map->width; ++x){
        for(uint32_t y = 0; y != map->height; ++y){
            uint32_t idx = map->width * y + x;
            LinceTransform* transform = array_get(&map->transforms, idx);

            // Don't draw off-screen tiles
            LinceTransform scrt;
            LincePoint scr = LinceAppGetWindowSize();
            LinceTransformToScreen(&scrt, transform, cam, scr.x, scr.y);
            if(
                scrt.x+scrt.w/2.0f < -1.0f || scrt.x-scrt.w/2.0f > 1.0f ||
                scrt.y+scrt.h/2.0f < -1.0f || scrt.y-scrt.h/2.0f > 1.0f){
                continue;
            }

            uint32_t grid_idx = *(uint32_t*)array_get(&map->indices, idx);
            LinceRect* uv = array_get(&map->tileset->coords, grid_idx);
            if(!uv) {
                // Tileset has no tile with the given index.
                // Easy way to have empty tiles in a tilemap - give them a very large index
                continue;
            }
            LinceDrawSpriteTile(&map->sprite, transform, uv, NULL);
        }
    }
}

struct nk_context* tme_get_nk(){
    return LinceUIGetNkContext(LinceGetApp()->ui);
}


void tme_nk_input_int(struct nk_context* ctx, int* value, int minvalue, int maxvalue){
    static char buf[25] = {0};
    snprintf(buf, sizeof(buf)-1, "%d", *value);
    nk_edit_string_zero_terminated (ctx, NK_EDIT_FIELD, buf, sizeof(buf) - 1, nk_filter_decimal);
    char* end;
    int result = (int)strtol(buf, &end, 10);

    if(end == buf || *end != '\0' || (result > maxvalue) || (result < minvalue)){
        return;
    }
    *value = result;
}


void tme_handle_input(float dt){

    if(nk_window_is_any_hovered(tme_get_nk())){
        // Ignore mouse input over nuklear windows
        return;
    }

    if(LinceIsMouseButtonPressed(LinceMouseButton_Left)
        && STATE.edit_tmap_tile != -1
        && STATE.draw_tset_tile != -1
        && STATE.tmap_loaded
    ){
        uint32_t idx = (uint32_t)STATE.edit_tmap_tile;
        uint32_t value = STATE.draw_tset_tile;
        array_set(&STATE.tmap->indices, &value, idx);
        STATE.tmap_saved = LinceFalse;
    }

    /* ===== MOUSE DRAG ===== */
    if(LinceIsMouseButtonPressed(LinceMouseButton_Right)
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


void tme_find_tile_pointed_by_mouse(){
    if(!STATE.tmap_loaded) return;

    LinceTilemap* map = STATE.tmap;
    LincePoint pos = LinceGetMousePosWorld(STATE.camera);

    float dx = (float)map->width  / 2.0f - ( ((map->pos.x - pos.x) / map->scale.x) + 0.0f);
    float dy = (float)map->height / 2.0f - ( ((map->pos.y - pos.y) / map->scale.y) + 0.0f);

    if(dx > 0.0 && dx < (float)map->width && dy > 0.0 && dy < (float)map->height){
        STATE.edit_tmap_tile = map->width * (uint32_t)dy + (uint32_t)dx;
    } else {
        STATE.edit_tmap_tile = -1;
    }
}


void tme_draw_tile_brush_edges(LinceTransform* transform){
    float brush_thickness = STATE.camera->zoom * STATE.grid_thickness;

    LinceSprite brush_edge_sprite = {.color = {0,0,1}, .zorder=1.0f};

    LinceTransform brush_edges[4] = {
        { // LEFT
            .x = transform->x - transform->w/2.0f,
            .y = transform->y,
            .w = brush_thickness,
            .h = transform->h,
            .coords = LinceCoordSystem_World
        },
        { // RIGHT
            .x = transform->x + transform->w/2.0f,
            .y = transform->y,
            .w = brush_thickness,
            .h = transform->h,
            .coords = LinceCoordSystem_World
        },
        { // TOP
            .x = transform->x,
            .y = transform->y + transform->h/2.0f,
            .w = transform->w,
            .h = brush_thickness,
            .coords = LinceCoordSystem_World
        },
        { // BOTTOm
            .x = transform->x,
            .y = transform->y - transform->h/2.0f,
            .w = transform->w,
            .h = brush_thickness,
            .coords = LinceCoordSystem_World
        },
    };

    for(uint32_t i = 0; i != 4; ++i){
        LinceDrawSprite(&brush_edge_sprite, &brush_edges[i], NULL);
    }
}


void tme_draw_tile_brush(){
    if(!STATE.tmap_loaded || STATE.edit_tmap_tile == -1) return;

    LinceTransform* transform = array_get(&STATE.tmap->transforms, (uint32_t)STATE.edit_tmap_tile);
    if(!transform) return;

    tme_draw_tile_brush_edges(transform);

    LinceSprite brush_tile_sprite = {
        .color = {1.0, 1.0, 1.0},
        .alpha=0.5,
        .zorder = 0.99f,
        .texture = STATE.tset_tex,
        .flags = LinceSprite_UseAlpha | LinceSprite_FlipY
    };
    LinceRect* uv = array_get(&STATE.tset->coords, STATE.draw_tset_tile);
    if(!uv) return;

    LinceDrawSpriteTile(&brush_tile_sprite, transform, uv, NULL);

}


void tme_draw_map_background(){
    if(!STATE.tmap_loaded) return;

    LinceSprite map_area = {
        .color  = {0.2, 0.2, 0.2},
        .zorder = 0.0
    };

    LinceTransform transform = {
        .x = STATE.tmap->pos.x,
        .y = STATE.tmap->pos.y,
        .w = (float)STATE.tmap->width  * STATE.tmap->scale.x * 1.00f,
        .h = (float)STATE.tmap->height * STATE.tmap->scale.y * 1.00f,
        .coords = LinceCoordSystem_World
    };

    LinceDrawSprite(&map_area, &transform, NULL);
}

void tme_draw_map_grid(){

    float grid_thickness = (STATE.camera->zoom < 10.0f) ? (STATE.grid_thickness * STATE.camera->zoom) : 0.0f;

    // Draw vertical grid lines
    for(uint32_t i = 0; i != STATE.tmap->width; ++i){
        LinceTransform line_transform = {
            .x = STATE.tmap->pos.x + STATE.tmap->scale.x * ((float)i - (float)STATE.tmap->width/2.0f),
            .y = STATE.tmap->pos.y,
            .w = grid_thickness,
            .h = (float)STATE.tmap->height * STATE.tmap->scale.y,
            .coords = LinceCoordSystem_World
        };
        LinceSprite line_sprite = {
            .color = {1,1,1}, .alpha=0.5, .flags = LinceSprite_UseAlpha, .zorder=1.0f
        };
        LinceDrawSprite(&line_sprite, &line_transform, NULL);
    }

    // Draw horizontal grid lines
    for(uint32_t i = 0; i != STATE.tmap->height; ++i){
        LinceTransform line_transform = {
            .x = STATE.tmap->pos.x,
            .y = STATE.tmap->pos.y + STATE.tmap->scale.y * ((float)i - (float)STATE.tmap->height/2.0f),
            .w = (float)STATE.tmap->width * STATE.tmap->scale.x,
            .h = grid_thickness,
            .coords = LinceCoordSystem_World
        };
        LinceSprite line_sprite = {
            .color = {1,1,1}, .alpha=0.5, .flags = LinceSprite_UseAlpha, .zorder=1.0f
        };
        LinceDrawSprite(&line_sprite, &line_transform, NULL);
    }
}

void tme_gui_show_load_tset_dialog(){

    nfdchar_t *outpath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outpath);

    if(result != NFD_OKAY){
        /// TODO: Show messagebox with error (+nfd error) if user didn't cancel either
        return;
    }

    LinceTexture* new_tex;
    LinceTileset new_tset;

    new_tex = LinceLoadTexture(outpath, LinceTexture_FlipY);
    if(!new_tex){
        /// TODO: show message box with error
        free(outpath);
    }

    void* init_success = LinceTilesetInit(&new_tset, new_tex, (uint32_t)STATE.tilesize, (uint32_t)STATE.tilesize);
    if(!init_success){
        /// TODO: show message box with error
        free(outpath);
        LinceDeleteTexture(new_tex);
    }

    tme_flip_tileset_coords(&new_tset);

    if(string_ok(STATE.tset_path)) string_free(&STATE.tset_path);
    if(STATE.tset_loaded) {
        LinceDeleteTexture(STATE.tset_tex);
        LinceTilesetUninit(STATE.tset);
    }

    STATE.tset_path = string_from_chars(outpath, strlen(outpath));
    STATE.tset_tex  = new_tex;
    *STATE.tset     = new_tset;
    STATE.tset_loaded = LinceTrue;
    STATE.draw_tset_tile = -1;
    STATE.edit_tmap_tile = -1;
    free(outpath);

    if(STATE.tmap_loaded){
        LinceTilemapUseTileset(STATE.tmap, STATE.tset);
    }

}

void tme_gui_show_load_tmap_dialog(){
    nfdchar_t *outpath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outpath);

    if(result != NFD_OKAY){
        /// TODO: Show messagebox with error (+nfd error) if user didn't cancel either
        return;
    }

    string_t tmap_path = string_from_chars(outpath, strlen(outpath));
    free(outpath);

    LinceTilemap new_tmap = {0};
    string_t tset_name = {0};
    LinceBool success = tme_load_tilemap(&new_tmap, tmap_path, &tset_name);
    if(!success) return;

    if(STATE.tmap_loaded){
        LinceTilemapUninit(STATE.tmap);
    }

    if(string_ok(tset_name)){
        if(string_ok(STATE.tset_name)){
            string_free(&STATE.tset_name);
        }
        STATE.tset_name = tset_name;
    }

    if(string_ok(STATE.tmap_path)){
        string_free(&STATE.tmap_path);
    }

    LINCE_INFO("LOADED NEW TILEMAP FROM %s", tmap_path.str);
    *STATE.tmap = new_tmap;
    STATE.tmap_saved = LinceTrue;
    STATE.edit_tmap_tile = -1;
    STATE.tmap_path = tmap_path;
    STATE.tmap_loaded = LinceTrue;
    LinceTilemapUseTileset(STATE.tmap, STATE.tset);
}

void tme_show_save_tmap_dialog(){
    nfdchar_t *outpath = NULL;
    nfdresult_t result = NFD_SaveDialog(NULL, NULL, &outpath);

    if(result != NFD_OKAY){
        /// TODO: Show messagebox with error (+nfd error) if user didn't cancel either
        return;
    }

    if(string_ok(STATE.tmap_path)){
        string_free(&STATE.tmap_path);
    }

    STATE.tmap_path = string_from_chars(outpath, strlen(outpath));
    STATE.tmap_saved = tme_write_tilemap(STATE.tmap, STATE.tmap_path, STATE.tset_name);

    free(outpath);
}


void tme_gui_draw_new_tmap_menu(){
    void* ctx = tme_get_nk();

    STATE.show_new_tmap_menu = nk_begin(
        ctx, "Create Tilemap",
        nk_rect(STATE.wsize.x/2-200, STATE.wsize.y/2-150, 200, 150),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE
    );

    if(STATE.show_new_tmap_menu){

        static int width = 10;
        static int height = 10;

        nk_layout_row_dynamic(ctx, 30, 2);
        nk_label(ctx, "Width", NK_TEXT_LEFT);
        tme_nk_input_int(ctx, &width, 1, 10000);

        nk_layout_row_dynamic(ctx, 30, 2);
        nk_label(ctx, "Height", NK_TEXT_LEFT);
        tme_nk_input_int(ctx, &height, 1, 10000);

        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "OK")) {
            LinceTilemap new_tmap = {
                .width  = (uint32_t)width,
                .height = (uint32_t)height,
                .scale  = {1.0, 1.0},
                .pos    = {0.0, 0.0}
            };
            uint32_t* map_grid = LinceCalloc(sizeof(uint32_t) * width * height);
            void* init_success = LinceTilemapInit(&new_tmap, map_grid);
            
            if(init_success){
                if(STATE.tmap_loaded){
                    LinceTilemapUninit(STATE.tmap);
                }
                *STATE.tmap = new_tmap;
                LinceTilemapUseTileset(STATE.tmap, STATE.tset);
                STATE.show_new_tmap_menu = LinceFalse;
                STATE.tmap_loaded = LinceTrue;
                STATE.draw_tset_tile = -1;
                STATE.edit_tmap_tile = -1;
            }

            LinceFree(map_grid);
        }
    }
    nk_end(ctx);
}


void tme_gui_draw_topbar(){
    struct nk_context* ctx = tme_get_nk();

    if (nk_begin(ctx, "Topbar", nk_rect(0, 0, STATE.wsize.x, 40),
        NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        
        nk_layout_row_begin(ctx, NK_STATIC, 30, 4);
        {
            // NEW MAP
            nk_layout_row_push(ctx, 100);
            if (nk_button_label(ctx, "NEW")) {
                STATE.show_new_tmap_menu = LinceTrue;
            }
            
            // LOAD MAP
            nk_layout_row_push(ctx, 100);
            if (nk_button_label(ctx, "LOAD")) {
                if(STATE.tset_loaded){
                    tme_gui_show_load_tmap_dialog();
                }
            }

            // SAVE MAP
            nk_layout_row_push(ctx, 100);
            
            if(STATE.tmap_loaded && !STATE.tmap_saved){
                nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(0,76,148)));
                nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(0,100,190)));
            } else {
                nk_style_push_style_item(ctx, &ctx->style.button.normal, ctx->style.button.normal);
                nk_style_push_style_item(ctx, &ctx->style.button.active, ctx->style.button.active);
            }

            if (nk_button_label(ctx, "SAVE")) {
                if(STATE.tmap_loaded && !STATE.tmap_saved){
                    if(string_ok(STATE.tmap_path)){
                        STATE.tmap_saved = tme_write_tilemap(STATE.tmap, STATE.tmap_path, STATE.tset_name);
                    } else {
                        tme_show_save_tmap_dialog();
                    }
                }
            }

            nk_style_pop_style_item(ctx);
            nk_style_pop_style_item(ctx);

            // TOGGLE GRID LINES
            nk_layout_row_push(ctx, 100);
            int show_grid = !(int)STATE.show_grid;
            nk_checkbox_label(ctx, "Show grid", &show_grid);
            STATE.show_grid = !(LinceBool)show_grid;

        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}


void tme_gui_draw_sidebar(){
    void* ctx = tme_get_nk();

    const struct nk_style_button tile_btn_style = {
        .image_padding = {5.0, 5.0},
        .hover  = {.type = NK_STYLE_ITEM_COLOR, .data = {.color = {200,200,200,100}}},
        .active = {.type = NK_STYLE_ITEM_COLOR, .data = {.color = {200,200,200,100}}},
    };
    
    const struct nk_style_button tile_btn_style_chosen = {
        .image_padding = {5.0, 5.0},
        .normal = {.type = NK_STYLE_ITEM_COLOR, .data = {.color = {200,200,200,100}}},
        .hover  = {.type = NK_STYLE_ITEM_COLOR, .data = {.color = {200,200,200,100}}},
        .active = {.type = NK_STYLE_ITEM_COLOR, .data = {.color = {200,200,200,100}}},
    };

    if (nk_begin(ctx, "Tileset Sidebar", nk_rect(0, 40, 400, STATE.wsize.y-40),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_MINIMIZABLE)) {
        
        // LOAD TILESET
        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_button_label(ctx, "Load Tileset")) {
            tme_gui_show_load_tset_dialog();
        }
        // CHOOSE TILESET TILE SIZE
        nk_property_int(ctx, "Tile Size", 1, &STATE.tilesize, 128, 1, 1);

        // TILESET TILES
        if (nk_tree_push(ctx, NK_TREE_TAB, "Tiles", NK_MAXIMIZED)) {
            if(STATE.tset_loaded){
                
                struct nk_image tile_img = {
                    .handle = {.id = STATE.tset_tex->id},
                    .w = (nk_short)STATE.tset_tex->width,
                    .h = (nk_short)STATE.tset_tex->height,
                };
                
                uint32_t tilecount = 0;
                for(uint32_t y = 0; y != STATE.tset->ycells; ++y){
                    for(uint32_t x = 0; x != STATE.tset->xcells; ++x){
                        uint32_t idx = STATE.tset->xcells * y + x;
                        
                        // Display tiles in rows of four
                        if( (tilecount % 4) == 0 ) nk_layout_row_dynamic(ctx, 90, 4);

                        // Find section of tileset image to display
                        LinceRect* r = LinceTilesetGetTileCoords(STATE.tset, x, y);
                        tile_img.region[0] = (nk_ushort)(r->x * tile_img.w);
                        tile_img.region[1] = (nk_ushort)(r->y * tile_img.h);
                        tile_img.region[2] = (nk_ushort)(r->w * tile_img.w);
                        tile_img.region[3] = (nk_ushort)(r->h * tile_img.h);
                        
                        // Make chosen tile always be highlighted
                        const void* btn_style;
                        if(STATE.draw_tset_tile == (int)idx){
                            btn_style = &tile_btn_style_chosen;
                        } else btn_style = &tile_btn_style;
                        
                        if(nk_button_image_styled(ctx, btn_style, tile_img)){
                            STATE.draw_tset_tile = (int)idx;
                        }

                        tilecount++;
                    }
                }
            }   
            nk_tree_pop(ctx);
        }
    }
    nk_end(ctx);
}


void tme_draw_test_gui(){
    struct nk_context* ctx = tme_get_nk();
    static float value = 0;
    enum {EASY, HARD};
    static int op = EASY;

    if (nk_begin(ctx, "Show", nk_rect(550, 550, 220, 220),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
        // fixed widget pixel width
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button")) {
            // event handling
        }
        // fixed widget window ratio width
        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
        // custom widget pixel width
        nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push(ctx, 50);
            nk_label(ctx, "Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 110);
            nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}


void tme_draw_gui(){
    // tme_draw_test_gui();
    tme_gui_draw_topbar();
    tme_gui_draw_sidebar();

    if(STATE.show_new_tmap_menu){
        if(STATE.tset_loaded){
            tme_gui_draw_new_tmap_menu();
        } else STATE.show_new_tmap_menu = LinceFalse;
    }

}


void tme_init(){
    STATE.camera = LinceCreateCamera(LinceAppGetAspectRatio());
    STATE.tset   = LinceCalloc(sizeof(LinceTileset));
    STATE.tmap   = LinceCalloc(sizeof(LinceTilemap));
    STATE.tilesize = 16;
    STATE.draw_tset_tile = -1;
    STATE.edit_tmap_tile = -1;
    STATE.grid_thickness = 0.01f;
    STATE.show_grid = LinceTrue;
    STATE.tset_name = string_from_literal("tileset.toml");
}

void tme_update(float dt){
    
    tme_handle_input(dt);
    LinceUpdateCameraProjection(STATE.camera, LinceAppGetAspectRatio());
	LinceUpdateCamera(STATE.camera);

    // Get tile pointed by mouse 
    tme_find_tile_pointed_by_mouse();

    STATE.wsize = LinceAppGetWindowSize();
    if(STATE.tmap_loaded) STATE.tmap->sprite.flags = LinceSprite_FlipY;

    if(STATE.tset_loaded && STATE.tilesize != (int)STATE.tset->cellsize.x){
        // Tile size was changed by the user
        LinceTilesetUninit(STATE.tset);
        LinceTilesetInit(STATE.tset, STATE.tset_tex, (uint32_t)STATE.tilesize, (uint32_t)STATE.tilesize);
        tme_flip_tileset_coords(STATE.tset);
    }

    tme_draw_gui();

    LinceBeginRender(STATE.camera);

    if (STATE.tmap_loaded){
        tme_draw_tilemap(STATE.tmap, STATE.camera);
        tme_draw_tile_brush();
        tme_draw_map_background();
        if(STATE.show_grid) tme_draw_map_grid();
    }

    LinceEndRender();
}

void tme_event(LinceEvent* event){

    if(nk_window_is_any_hovered(tme_get_nk())){
        // Avoid drawing under nuklear windows
        return;
    }

    if(event->type == LinceEventType_MouseScroll){
        LinceMouseScrollEvent* scroll = event->data.mouse_scroll;    
        STATE.camera->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
        return;

    }
    
    if (event->type == LinceEventType_KeyRelease){
        LinceKeyReleaseEvent* ev = event->data.key_release;

        // ===== SAVE TILEMAP ===== 
        if(ev->keycode == LinceKey_s && (ev->mods & LinceKeyMod_Control)){
            if(STATE.tmap_loaded && !STATE.tmap_saved){
                if(string_ok(STATE.tmap_path)){
                    STATE.tmap_saved = tme_write_tilemap(STATE.tmap, STATE.tmap_path, STATE.tset_name);
                } else {
                    tme_show_save_tmap_dialog();
                }
            }
        }
    }
}

void tme_terminate(){
    if(STATE.tset_loaded) LinceTilesetUninit(STATE.tset);
    if(STATE.tmap_loaded) LinceTilemapUninit(STATE.tmap);
    if(string_ok(STATE.tmap_path)) string_free(&STATE.tmap_path);
    if(string_ok(STATE.tset_path)) string_free(&STATE.tset_path);
    if(string_ok(STATE.tset_name)) string_free(&STATE.tset_name);

    LinceFree(STATE.tset);
    LinceFree(STATE.tmap);
}

int main(){
    LinceApp* app = LinceGetApp();
    app->on_init = tme_init;
    app->on_update = tme_update;
    app->on_event = tme_event;
    app->on_terminate = tme_terminate;
    
    LinceAppSetConfigFile(string_scoped_lit("demos/tmapedit/project.toml"));
    LinceRun();
    return 0;
}