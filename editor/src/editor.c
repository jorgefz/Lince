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
#include <math.h>
#include <lince.h>
#include <lince/physics/boxcollider.h>

#define TAG_MAX 100
typedef struct LinceTag {
    char tag[TAG_MAX];
} LinceTag;


enum ECSComponents{
    Component_Tag,
    Component_BoxCollider,
    Component_Sprite,
    Component_Shader
};

/*
static const char* COMPNAMES[] = {
    "Tag",
    "BoxCollider",
    "Sprite",
    "Shader"
};
*/

typedef struct EditorState {
    LinceEntityRegistry* reg;
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


void BoxColliderGUI(struct nk_context* ctx, uint32_t entity_id){
    LinceBoxCollider* box = LinceGetEntityComponent(STATE.reg, entity_id, Component_BoxCollider);
    LinceSprite* sprite = LinceGetEntityComponent(STATE.reg, entity_id, Component_Sprite);
    if(!box) return;

    static int tree_id = 200;
    if (nk_tree_push_id(ctx, NK_TREE_TAB, "BoxCollider", NK_MINIMIZED, tree_id)) {
        nk_layout_row_dynamic(ctx, 30, 1);
        static nk_bool fixed = LinceTrue;
        if(nk_checkbox_label(ctx, "Fixed to sprite", &fixed)){
            if(!sprite) fixed = LinceFalse;
            if(fixed){
                box->x = sprite->x;
                box->y = sprite->y;
                box->w = sprite->w;
                box->h = sprite->h;
            }
        }
        nk_layout_row_dynamic(ctx, 30, 2);
        nk_property_float(ctx, "x", -10.0, &box->x, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "y", -10.0, &box->y, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "w",   0.0, &box->w, 10.0, 0.1, 0.1);
        nk_property_float(ctx, "h",   0.0, &box->h, 10.0, 0.1, 0.1);

        nk_tree_pop(ctx);
    }
}


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

void GUIShader(){
    // Separate section to create shaders
    // For entities, list to select from and give uniform values
}


void DrawGUI(){

    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context* ctx = ui->ctx;

    vec2 screen_size;
    LinceGetScreenSize(screen_size);
    float width = screen_size[0], height = screen_size[1];
    
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid15]->handle);
    
    /* TOP BAR */
    float topbar_h = 50;
    if (nk_begin(ctx, "TopBar", nk_rect(0,0, width, topbar_h), 0)){
        nk_layout_row_static(ctx, 30, 50, 3);
        nk_end(ctx);
    }

    /* SIDEBAR - ENTITIES */
    float sidebar_h = height - topbar_h;
    float sidebar_w = 250;
    if (nk_begin(ctx, "SideBar", nk_rect(0,topbar_h, sidebar_w, sidebar_h), 0)){
        nk_layout_row_dynamic(ctx, 30, 2);
        
        // Create Entity button
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

        // Tree of entities
        array_t query;
        array_init(&query, sizeof(uint32_t));
        LinceQueryEntities(STATE.reg, &query, 1, Component_Tag);

        // Number of entities
        nk_labelf(ctx, NK_TEXT_CENTERED,
            "Entities: %u", query.size);

        for(uint32_t i = 0; i != query.size; ++i){
            uint32_t id = *(uint32_t*)array_get(&query, i);
            LinceTag* tag = LinceGetEntityComponent(STATE.reg, id, Component_Tag);
            if (nk_tree_push_id(ctx, NK_TREE_TAB, tag->tag, NK_MINIMIZED, i)) {
                /* Entity options bar */
                nk_layout_row_dynamic(ctx, 30, 2);
                // Delete entity
                if(nk_button_label(ctx, "Delete")){
                    LinceDeleteEntity(STATE.reg, id);
                    nk_tree_pop(ctx);
                    continue;
                }

                /* Component data */
                SpriteGUI(ctx, id);
                BoxColliderGUI(ctx, id);
                

                nk_tree_pop(ctx);
            }
        }

        array_uninit(&query);
        nk_end(ctx);
    }

}

void DrawEntities(){
    array_t query;
    array_init(&query, sizeof(uint32_t));
    LinceQueryEntities(STATE.reg, &query, 1, Component_Sprite);

    LinceBeginScene(STATE.camera);
    for(uint32_t i = 0; i != query.size; ++i){
        uint32_t id = *(uint32_t*)array_get(&query, i);
        LinceSprite* sprite = LinceGetEntityComponent(STATE.reg, id, Component_Sprite);
        LinceDrawSprite(sprite, NULL);
    }
    LinceEndScene();

    array_uninit(&query);
}

void MoveCamera(float dt){
    static const float camera_speed = 1e-3; // units/frame
    const float ds = camera_speed * dt * STATE.camera->zoom;
    if(LinceIsKeyPressed(LinceKey_w)) STATE.camera->pos[1] += ds;
    if(LinceIsKeyPressed(LinceKey_s)) STATE.camera->pos[1] -= ds;
    if(LinceIsKeyPressed(LinceKey_d)) STATE.camera->pos[0] += ds;
    if(LinceIsKeyPressed(LinceKey_a)) STATE.camera->pos[0] -= ds;

    if(LinceIsMouseButtonPressed(LinceMouseButton_Left)
        // && LinceIsKeyPressed(LinceKey_LeftControl)
    ){
        vec2 mouse_pos;
        LinceGetMousePosWorld(mouse_pos, STATE.camera);
        if(!STATE.mouse_drag){
            STATE.mouse_drag_from[0] = mouse_pos[0];
            STATE.mouse_drag_from[1] = mouse_pos[1];
        } else {
            vec2 dr = { mouse_pos[0] - STATE.mouse_drag_from[0],
                        mouse_pos[1] - STATE.mouse_drag_from[1] };
            STATE.camera->pos[0] -= dr[0] * 1.5f;
            STATE.camera->pos[1] -= dr[1] * 1.5f;
        }
        STATE.mouse_drag = LinceTrue;

    } else {
        STATE.mouse_drag = LinceFalse;
    }
}


void EditorInit(){
    STATE.reg = LinceCreateEntityRegistry(
        3,
        sizeof(LinceBoxCollider),
        sizeof(LinceSprite),
        sizeof(LinceShader)
    );
    STATE.camera = LinceCreateCamera(LinceGetAspectRatio());
}

void EditorOnUpdate(float dt){
    MoveCamera(dt);
    LinceResizeCameraView(STATE.camera, LinceGetAspectRatio());
	LinceUpdateCamera(STATE.camera);
    DrawGUI();
    DrawEntities();
}

void EditorOnEvent(LinceEvent* event){

    if(event->type == LinceEventType_MouseScroll){
        LinceMouseScrollEvent* scroll = event->data.mouse_scroll;    
        STATE.camera->zoom *= powf(0.80, scroll->yoff); // * 0.5 * dt;
        return;
    }

}


void EditorTerminate(){
    LinceDestroyEntityRegistry(STATE.reg);
    LinceDeleteCamera(STATE.camera);
}