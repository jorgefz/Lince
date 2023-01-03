#include "ecs.h"

#include <stdarg.h>


/* Creates a registry that will be used to spawn entities */
LinceEntityRegistry* LinceCreateEntityRegistry(uint32_t component_count, ...){
    LINCE_ASSERT(component_count > 0, "Component count must be greater than zero");

    uint32_t max_components = LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT * sizeof(uint64_t) * 8;
    LINCE_ASSERT(component_count <= max_components,
        "Too many components, max is %u.", max_components);
    
    LinceEntityRegistry* reg = LinceMalloc(sizeof(LinceEntityRegistry));
    LINCE_ASSERT_ALLOC(reg, sizeof(LinceEntityRegistry));
    
    reg->component_count = component_count;
    reg->max_components = max_components;
    array_init(&reg->component_sizes, sizeof(uint32_t));
    array_init(&reg->component_offsets, sizeof(uint32_t));
    
    // Fetch component sizes from varargs
    va_list args;
    va_start(args, component_count);
    uint32_t entity_size = 0;
    for(uint32_t i = 0; i != component_count; ++i){
        uint32_t comp_size = va_arg(args, uint32_t);
        if(comp_size == 0){
            array_uninit(&reg->component_sizes);
            array_uninit(&reg->component_offsets);
            LinceFree(reg);
            LINCE_ASSERT(0, "Size of component #%u is zero", i);
        }
        array_push_back(&reg->component_sizes, &comp_size);
        array_push_back(&reg->component_offsets, &entity_size);
        entity_size += comp_size;
    }
    va_end(args);
    reg->entity_size = entity_size;
    reg->entity_count = 0;

    LINCE_INFO("Creating Entity Registry - %u components - %u bytes per entity",
        component_count, entity_size);
    
    array_init(&reg->entity_data, entity_size);
    array_init(&reg->entity_flags, sizeof(LinceEntityState));
    array_init(&reg->entity_masks, sizeof(LinceEntityMask));
    array_init(&reg->entity_pool, sizeof(uint32_t));
    
    return reg;
}

void LinceDestroyEntityRegistry(LinceEntityRegistry* reg){
    LINCE_ASSERT(reg, "NULL pointer");

    array_uninit(&reg->component_sizes);
    array_uninit(&reg->component_offsets);

    array_uninit(&reg->entity_data);
    array_uninit(&reg->entity_flags);
    array_uninit(&reg->entity_masks);
    array_uninit(&reg->entity_pool);

    LinceFree(reg);
}


uint32_t LinceCreateEntity(LinceEntityRegistry* reg){
    LINCE_ASSERT(reg, "NULL pointer");
    uint32_t id = 0;

    if(reg->entity_pool.size > 0){
        // Draw from pool of unused entities
        id = *(uint32_t*)array_back(&reg->entity_pool);
        array_pop_back(&reg->entity_pool);
        // Set alive flag
        uint32_t* flag = array_get(&reg->entity_flags, id);
        *flag = LinceEntityState_Active;
        return id;
    }

    // Allocate new entity
    id = reg->entity_count++;
    // LinceEntityMask mask = {0};
    LinceEntityState state = LinceEntityState_Active;
    array_push_back(&reg->entity_data,  NULL);
    array_push_back(&reg->entity_flags, &state);
    array_push_back(&reg->entity_masks, NULL);

    return id;
}

#define CheckEntityArgs(reg, entity_id, component_id) do{ \
    LINCE_ASSERT(reg, "NULL pointer"); \
    LINCE_ASSERT(entity_id < reg->entity_count, "Invalid entity ID"); \
    LINCE_ASSERT(component_id < reg->component_count, "Invalid component ID"); \
    LINCE_ASSERT(*(uint32_t*)array_get(&reg->entity_flags, entity_id) & LinceEntityState_Active, \
        "Entity is inactive"); \
} while(0) \

void LinceDeleteEntity(LinceEntityRegistry* reg, uint32_t entity_id){
    LINCE_ASSERT(reg, "NULL pointer");
    LINCE_ASSERT(entity_id < reg->entity_count, "Entity ID out of bounds");

    LinceEntityState* flag = array_get(&reg->entity_flags, entity_id);
    *flag = 0;

    uint64_t* mask = array_get(&reg->entity_masks, entity_id);
    memset(mask, 0, sizeof(LinceEntityMask));

    array_push_back(&reg->entity_pool, &entity_id);
}

void LinceAddEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id, void* data){
    CheckEntityArgs(reg, entity_id, component_id);
    LINCE_ASSERT(data, "NULL pointer");

    // Copy component data to entity data array
    uint8_t* entity_data = (uint8_t*)array_get(&reg->entity_data, entity_id);
    uint8_t* comp_data = entity_data + *(uint32_t*)array_get(&reg->component_offsets, component_id);
    uint32_t bytes = *(uint32_t*)array_get(&reg->component_sizes, component_id);
    memmove(comp_data, data, bytes);
    
    // Set flag in component mask
    uint64_t* mask = array_get(&reg->entity_masks, entity_id);
    uint32_t i   = component_id / 64; // index in the array
    uint32_t pos = component_id % 64; // bit position within the array element
    mask[i] |= (1 << pos);
}

LinceBool LinceHasEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id){
    CheckEntityArgs(reg, entity_id, component_id);
    uint64_t* mask = array_get(&reg->entity_masks, entity_id);
    uint32_t i   = component_id / 64; // index in the array
    uint32_t pos = component_id % 64; // bit position within the array element
    return mask[i] & (1 << pos);
}

void* LinceGetEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id){
    CheckEntityArgs(reg, entity_id, component_id);
    if(LinceHasEntityComponent(reg, entity_id, component_id) == LinceFalse){
        return NULL;
    }
    uint8_t* entity_data = (uint8_t*)array_get(&reg->entity_data, entity_id);
    uint8_t* comp_data = entity_data + *(uint32_t*)array_get(&reg->component_offsets, component_id);
    return comp_data;
}

void LinceDeleteEntityComponent(LinceEntityRegistry* reg, uint32_t entity_id, uint32_t component_id){
    CheckEntityArgs(reg, entity_id, component_id);
    uint64_t* mask = array_get(&reg->entity_masks, entity_id);
    uint32_t i   = component_id / 64; // index in the array
    uint32_t pos = component_id % 64; // bit position within the array element
    mask[i] &= ~(1 << pos);
}


uint32_t LinceQueryEntities(LinceEntityRegistry* reg, array_t* query, uint32_t component_count, ...){
    LINCE_ASSERT(reg && query, "NULL pointer");
    uint32_t query_count = 0;
    LinceEntityMask query_mask = {0};
    va_list args;

    // Build component mask
    va_start(args, component_count);
    for(uint32_t i = 0; i != component_count; ++i){
        uint32_t comp_id = va_arg(args, uint32_t);
        uint32_t ind = comp_id / 64; // index in the array
        uint32_t pos = comp_id % 64; // bit position within the array element
        query_mask[ind] |= (1 << pos);
    }
    va_end(args);

    printf("Query mask: %d %d %d\n", !!(query_mask[0]&(1<<0)), !!(query_mask[0]&(1<<1)), !!(query_mask[0]&(1<<2)));
    
    // Query entities that match bits in search mask
    for(uint32_t id = 0; id != reg->entity_count; ++id){
        uint32_t active = *(uint32_t*)array_get(&reg->entity_flags, id) & LinceEntityState_Active;
        uint64_t* entity_mask = array_get(&reg->entity_masks, id);
        
        int masks_match = 1;
        for(uint32_t i = 0; i != LINCE_MAX_ENTITY_COMPONENTS_U64_COUNT; ++i){
            masks_match = masks_match && query_mask[i] == (query_mask[i] & entity_mask[i]); 
        }
        
        if(active && masks_match){
            array_push_back(query, &id);
            query_count++;
        }
    }


    return query_count;
}