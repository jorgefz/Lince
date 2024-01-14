#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <time.h>
#include <lince/renderer/renderer.h>
#include <lince/entity/entity.h>

typedef struct Position { float x, y; } Position;
typedef struct Velocity { float vx, vy; } Velocity;
typedef struct Sprite   { LinceSprite data; } Sprite;
typedef struct Timer    { float start, end, step, counter; } Timer;
typedef struct BoxCollider { float x, y, w, h; } BoxCollider;

typedef enum {CompPosition, CompVelocity, CompSprite, CompTimer, CompBoxCollider} Components;


void test_entity(void** state){
    (void)state;

    LinceEntityRegistry* reg;

    // These should cause an error in debug mode
    // reg = LinceCreateEntityRegistry(0); // error: zero components
    // reg = LinceCreateEntityRegistry(65); // error: too many components (max 64)
    // reg = LinceCreateEntityRegistry(3, /*sizes:*/ 1, 2, 0); // error: component is zero-sized

    uint32_t comp_sizes[] = {sizeof(struct Position), sizeof(struct Velocity), sizeof(struct Sprite)};
    uint32_t comp_offsets[] = {0, comp_sizes[0], comp_sizes[0]+comp_sizes[1]}; 
    uint32_t comp_num = 3;

    // Create Registry
    reg = LinceCreateEntityRegistry(comp_num, comp_sizes[0], comp_sizes[1], comp_sizes[2]);

    assert_true(reg->component_count == comp_num);
    assert_true(reg->component_sizes.size == comp_num);
    
    for(uint32_t i=0; i != comp_num; ++i){
        assert_true(comp_sizes[i] == *(uint32_t*)array_get(&reg->component_sizes, i));
        assert_true(comp_offsets[i] == *(uint32_t*)array_get(&reg->component_offsets, i));
    }

    // Create Entity
    uint32_t id = LinceCreateEntity(reg);
    assert_true(id == 0);
    assert_true(reg->entity_count == 1);
    assert_true(reg->entity_masks.size == 1);
    assert_true(reg->entity_flags.size == 1);
    assert_true(reg->entity_data.size == 1);
    
    uint64_t* mask = array_get(&reg->entity_masks, id);
    LinceEntityState* flag = array_get(&reg->entity_flags, id);
    assert_true(*flag & LinceEntityState_Active);
    assert_true(*mask == 0);

    // Create second entity
    uint32_t id2 = LinceCreateEntity(reg);
    assert_true(id2 == 1);
    assert_true(reg->entity_count == 2);
    assert_true(reg->entity_masks.size == 2);
    assert_true(reg->entity_flags.size == 2);
    assert_true(reg->entity_data.size == 2);

    // Delete first entity
    flag = array_get(&reg->entity_flags, id);
    mask = array_get(&reg->entity_masks, id);
    LinceDeleteEntity(reg, id);
    assert_true(*flag == 0);
    assert_true(*mask == 0);
    assert_true(reg->entity_pool.size == 1);
    assert_true(*(uint32_t*)array_get(&reg->entity_pool, 0) == id);

    // Create third entity - reuse ID for first entity
    uint32_t id3 = LinceCreateEntity(reg);
    assert_true(id3 == id);
    assert_true(reg->entity_count == 2);
    assert_true(reg->entity_masks.size == 2);
    assert_true(reg->entity_flags.size == 2);
    assert_true(reg->entity_data.size == 2);

    // Add components to entity third entity
    LinceAddEntityComponent(reg, id3, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id3, CompSprite,   &(struct Sprite){0});

    assert_true(LinceHasEntityComponent(reg, id3, CompPosition));
    assert_true(LinceHasEntityComponent(reg, id3, CompSprite));
    assert_false(LinceHasEntityComponent(reg, id3, CompVelocity));
    
    struct Position* pos = LinceGetEntityComponent(reg, id3, CompPosition);
    struct Velocity* vel = LinceGetEntityComponent(reg, id3, CompVelocity);
    struct Sprite* sprite = LinceGetEntityComponent(reg, id3, CompSprite);
    
    assert_non_null(pos);
    assert_null(vel);
    assert_non_null(sprite);
    assert_true(pos->x == 1.0);
    assert_true(pos->y == 2.0);

    // Add components to second entity
    LinceAddEntityComponent(reg, id2, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id2, CompVelocity, &(struct Velocity){-1.0, 5.0});
    
    assert_true(LinceHasEntityComponent(reg, id2, CompPosition));
    assert_true(LinceHasEntityComponent(reg, id2, CompVelocity));
    assert_false(LinceHasEntityComponent(reg, id2, CompSprite));
    
    pos = LinceGetEntityComponent(reg, id2, CompPosition);
    vel = LinceGetEntityComponent(reg, id2, CompVelocity);
    sprite = LinceGetEntityComponent(reg, id2, CompSprite);

    assert_non_null(pos);
    assert_non_null(vel);
    assert_null(sprite);
    assert_true(pos->x == 1.0);
    assert_true(pos->y == 2.0);
    assert_true(vel->vx == -1.0);
    assert_true(vel->vy == 5.0);

    // Delete component from second entity
    LinceRemoveEntityComponent(reg, id2, CompVelocity);
    assert_false(LinceHasEntityComponent(reg, id2, CompVelocity));

    // Query entities
    // -- Query sprite component
    array_t query_result_sprite;
    array_init(&query_result_sprite, sizeof(uint32_t));
    uint32_t count_sprite = LinceQueryEntities(reg, &query_result_sprite, 1, CompSprite);
    
    assert_true(count_sprite == 1);
    assert_true(query_result_sprite.size == 1);
    assert_true(*(uint32_t*)array_get(&query_result_sprite, 0) == id3);
    array_uninit(&query_result_sprite);
    
    // -- query position component
    array_t query_result_pos;
    array_init(&query_result_pos, sizeof(uint32_t));
    uint32_t count_pos = LinceQueryEntities(reg, &query_result_pos, 1, CompPosition);
    
    assert_true(count_pos == 2);
    assert_true(query_result_pos.size == 2);
    assert_true(*(uint32_t*)array_get(&query_result_pos, 0) == id3);
    assert_true(*(uint32_t*)array_get(&query_result_pos, 1) == id2);
    array_uninit(&query_result_pos);
    
    // -- query sprite and position components
    array_t query_result_sprite_vel;
    array_init(&query_result_sprite_vel, sizeof(uint32_t));
    uint32_t count_sprite_vel = LinceQueryEntities(reg, &query_result_sprite_vel, 2, CompSprite, CompPosition);
    
    assert_true(count_sprite_vel == 1);
    assert_true(query_result_sprite_vel.size == 1);
    array_uninit(&query_result_sprite_vel);

    // -- query velocity and position components
    array_t query_result_pos_vel;
    array_init(&query_result_pos_vel, sizeof(uint32_t));
    uint32_t count_pos_vel = LinceQueryEntities(reg, &query_result_pos_vel, 2, CompVelocity, CompPosition);
   
    assert_true(count_pos_vel == 0);
    assert_true(query_result_pos_vel.size == 0); 
    array_uninit(&query_result_pos_vel);

    // Destroy
    LinceDestroyEntityRegistry(reg);
}
