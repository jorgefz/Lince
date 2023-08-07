#include <lince/renderer/renderer.h>
#include <lince/entity/entity.h>

#include "test.h"
#include "targets.h"

#include <time.h>

typedef struct Position { float x, y; } Position;
typedef struct Velocity { float vx, vy; } Velocity;
typedef struct Sprite   { LinceSprite data; } Sprite;
typedef struct Timer    { float start, end, step, counter; } Timer;
typedef struct BoxCollider { float x, y, w, h; } BoxCollider;

typedef enum {CompPosition, CompVelocity, CompSprite, CompTimer, CompBoxCollider} Components;


int test_entity_performance(){

    LinceEntityRegistry* reg;
    #define SIZES sizeof(Position), sizeof(Velocity), sizeof(Sprite), sizeof(Timer), sizeof(BoxCollider)
    void* instances[5] = {
        &(Position){2.0,3.0},
        &(Velocity){3.0,4.0},
        &(Sprite){0},
        &(Timer){0.0, 100.0, 0.1, 0.0},
        &(BoxCollider){0.0, -1.0, 1.0, 5.0},
    };

    srand(time(NULL));

    reg = LinceCreateEntityRegistry(5, SIZES);

    uint32_t num = 1000000;
    array_t entities;
    array_init(&entities, sizeof(uint32_t));
    array_resize(&entities, num);

    TEST_CLOCK_START(time);
    for(uint32_t i = 0; i != num; ++i){
        uint32_t id = LinceCreateEntity(reg);
        array_set(&entities, &i, id);
        int r = rand() % 5;
        LinceAddEntityComponent(reg, id, r, instances[r]);
    }
    long int op_num = (long int)num;
    TEST_CLOCK_END(time, op_num);

    TEST_CLOCK_START(time_query);
    array_t query_result;
    array_init(&query_result, sizeof(uint32_t));
    array_resize(&query_result, num);
    
    LinceQueryEntities(reg, &query_result, 1, 0);
    LinceQueryEntities(reg, &query_result, 1, 1);
    LinceQueryEntities(reg, &query_result, 1, 2);
    LinceQueryEntities(reg, &query_result, 1, 3);
    LinceQueryEntities(reg, &query_result, 1, 4);
    
    array_uninit(&query_result);
    long int query_op = 5;
    TEST_CLOCK_END(time_query, query_op);

    array_uninit(&entities);
    LinceDestroyEntityRegistry(reg);
    return TEST_PASS;
}


int test_entity(){

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

    TEST_ASSERT(reg->component_count == comp_num && reg->component_sizes.size == comp_num,
        "Component count is not three");
    TEST_ASSERT( comp_sizes[0] == *(uint32_t*)array_get(&reg->component_sizes, 0), "1st component size is wrong");
    TEST_ASSERT( comp_sizes[1] == *(uint32_t*)array_get(&reg->component_sizes, 1), "2nd component size is wrong");
    TEST_ASSERT( comp_sizes[2] == *(uint32_t*)array_get(&reg->component_sizes, 2), "3rd component size is wrong");

    TEST_ASSERT( comp_offsets[0] == *(uint32_t*)array_get(&reg->component_offsets, 0), "1st component offset is wrong");
    TEST_ASSERT( comp_offsets[1] == *(uint32_t*)array_get(&reg->component_offsets, 1), "2nd component offset is wrong");
    TEST_ASSERT( comp_offsets[2] == *(uint32_t*)array_get(&reg->component_offsets, 2), "3rd component offset is wrong");

    // Create Entity
    uint32_t id = LinceCreateEntity(reg);
    TEST_ASSERT(
        id == 0 && reg->entity_count == 1
        && reg->entity_masks.size == 1 && reg->entity_flags.size == 1
        && reg->entity_data.size == 1,
        "Failed to create entity"
    );
    uint64_t* mask = array_get(&reg->entity_masks, id);
    LinceEntityState* flag = array_get(&reg->entity_flags, id);
    TEST_ASSERT(*flag & LinceEntityState_Active, "Failed to setup entity flags");
    TEST_ASSERT(*mask == 0, "Failed to setup entity mask");

    // Create second entity
    uint32_t id2 = LinceCreateEntity(reg);
    TEST_ASSERT(
        id2 == 1 
        && reg->entity_count == 2      && reg->entity_masks.size == 2
        && reg->entity_flags.size == 2 && reg->entity_data.size == 2,
        "Failed to create second entity"
    );

    // Delete first entity
    LinceDeleteEntity(reg, id);
    TEST_ASSERT(
        *flag == 0 && *mask == 0 && reg->entity_pool.size == 1
        && *(uint32_t*)array_get(&reg->entity_pool, 0) == id,
        "Failed to delete entity"
    );

    // Create third entity - should recycle first entity (with id 0)
    uint32_t id3 = LinceCreateEntity(reg);
    TEST_ASSERT(
        id3 == id
        && reg->entity_count == 2      && reg->entity_masks.size == 2 
        && reg->entity_flags.size == 2 && reg->entity_data.size == 2,
        "Failed to create third entity"
    );

    // Add components to entity third entity
    LinceAddEntityComponent(reg, id3, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id3, CompSprite,   &(struct Sprite){0});

    TEST_ASSERT(LinceHasEntityComponent(reg, id3, CompPosition),
        "Failed to add component to entity");
    TEST_ASSERT(LinceHasEntityComponent(reg, id3, CompSprite),
        "Failed to add component to entity");
    TEST_ASSERT(LinceHasEntityComponent(reg, id3, CompVelocity) == LinceFalse,
        "Added wrong component to entity");

    struct Position* pos = LinceGetEntityComponent(reg, id3, CompPosition);
    TEST_ASSERT(pos && pos->x==1.0 && pos->y==2.0, "Failed to retrieve position component");
    struct Velocity* vel = LinceGetEntityComponent(reg, id3, CompVelocity);
    TEST_ASSERT(!vel, "Retrieved uninitialised velocity component");
    struct Sprite* sprite = LinceGetEntityComponent(reg, id3, CompSprite);
    TEST_ASSERT(sprite, "Failed to retrieve sprite component");

    // Add components to second entity
    LinceAddEntityComponent(reg, id2, CompPosition, &(struct Position){1.0, 2.0});
    LinceAddEntityComponent(reg, id2, CompVelocity, &(struct Velocity){-1.0, 5.0});

    TEST_ASSERT(LinceHasEntityComponent(reg, id2, CompPosition),
        "Failed to add component to entity");
    TEST_ASSERT(LinceHasEntityComponent(reg, id2, CompVelocity),
        "Failed to add component to entity");
    TEST_ASSERT(LinceHasEntityComponent(reg, id2, CompSprite) == LinceFalse,
        "Added wrong component to entity");

    pos = LinceGetEntityComponent(reg, id2, CompPosition);
    TEST_ASSERT(pos && pos->x==1.0 && pos->y==2.0, "Failed to retrieve position component");
    sprite = LinceGetEntityComponent(reg, id2, CompSprite);
    TEST_ASSERT(!sprite, "Retrieved uninitialised sprite component");
    vel = LinceGetEntityComponent(reg, id2, CompVelocity);
    TEST_ASSERT(vel, "Failed to retrieve velocity component");
    
    // Delete component from second entity
    LinceRemoveEntityComponent(reg, id2, CompVelocity);
    TEST_ASSERT(LinceHasEntityComponent(reg, id2, CompVelocity) == LinceFalse,
        "Failed to delete component from entity");

    // Query entities
    // -- query sprite component
    array_t query_result_sprite;
    array_init(&query_result_sprite, sizeof(uint32_t));
    uint32_t count_sprite = LinceQueryEntities(reg, &query_result_sprite, 1, CompSprite);
    TEST_ASSERT(
        count_sprite == 1 && query_result_sprite.size == 1
        && *(uint32_t*)array_get(&query_result_sprite, 0) == id3,
        "Failed to query entities with Sprite component"
    );
    array_uninit(&query_result_sprite);
    
    // -- query position component
    array_t query_result_pos;
    array_init(&query_result_pos, sizeof(uint32_t));
    uint32_t count_pos = LinceQueryEntities(reg, &query_result_pos, 1, CompPosition);
    TEST_ASSERT(
        count_pos == 2 && query_result_pos.size == 2
        && *(uint32_t*)array_get(&query_result_pos, 0) == id3
        && *(uint32_t*)array_get(&query_result_pos, 1) == id2,
        "Failed to query entities with Position component"
    );
    array_uninit(&query_result_pos);
    
    // -- query sprite and position components
    array_t query_result_sprite_vel;
    array_init(&query_result_sprite_vel, sizeof(uint32_t));
    uint32_t count_sprite_vel = LinceQueryEntities(reg, &query_result_sprite_vel, 2, CompSprite, CompPosition);
    TEST_ASSERT(
        count_sprite_vel == 1 && query_result_sprite_vel.size == 1,
        "Failed to query entities with Sprite and Position components"
    );
    array_uninit(&query_result_sprite_vel);

    // -- query velocity and position components
    array_t query_result_pos_vel;
    array_init(&query_result_pos_vel, sizeof(uint32_t));
    uint32_t count_pos_vel = LinceQueryEntities(reg, &query_result_pos_vel, 2, CompVelocity, CompPosition);
    TEST_ASSERT(
        count_pos_vel == 0 && query_result_pos_vel.size == 0,
        "Failed to query entities with Velocity and Position components"
    );
    array_uninit(&query_result_pos_vel);

    // Destroy
    LinceDestroyEntityRegistry(reg);

    return TEST_PASS;
}
