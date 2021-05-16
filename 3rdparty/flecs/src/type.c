#include "private_api.h"

ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    /* Walk from back to front, as prefabs are always located
     * at the end of the type. */
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = array[i];

        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            ecs_entity_t prefab = e & ECS_COMPONENT_MASK;
            ecs_type_t prefab_type = ecs_get_type(world, prefab);

            if (prefab == previous) {
                continue;
            }

            if (ecs_type_owns_entity(
                world, prefab_type, component, true)) 
            {
                return prefab;
            } else {
                prefab = ecs_find_entity_in_prefabs(
                    world, prefab, prefab_type, component, entity);
                if (prefab) {
                    return prefab;
                }
            }
        } else {
            /* If this is not a prefab, the following entities won't
                * be prefabs either because the array is sorted, and
                * the prefab bit is 2^63 which ensures that prefabs are
                * guaranteed to be the last entities in the type */
            break;
        }
    }

    return 0;
}

/* -- Private functions -- */

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_type_t type_1,
    ecs_type_t type_2,
    bool match_all,
    bool match_prefab)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_get_stage(&world);
    
    if (!type_1) {
        return 0;
    }

    ecs_assert(type_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_1 == type_2) {
        return *(ecs_vector_first(type_1, ecs_entity_t));
    }

    int32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1, ecs_entity_t);
    ecs_entity_t *t2_array = ecs_vector_first(type_2, ecs_entity_t);

    ecs_assert(t1_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t2_array != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t t1_count = ecs_vector_count(type_1);
    int32_t t2_count = ecs_vector_count(type_2);

    for (i_2 = 0; i_2 < t2_count; i_2 ++) {
        ecs_entity_t e2 = t2_array[i_2];

        if (i_1 >= t1_count) {
            return 0;
        }

        e1 = t1_array[i_1];

        if (e2 > e1) {
            do {
                i_1 ++;
                if (i_1 >= t1_count) {
                    return 0;
                }
                e1 = t1_array[i_1];
            } while (e2 > e1);
        }

        if (e1 != e2) {
            if (match_prefab && e2 != 
                ecs_typeid(EcsName) && e2 != 
                EcsPrefab && e2 != 
                EcsDisabled) 
            {
                if (ecs_find_entity_in_prefabs(world, 0, type_1, e2, 0)) {
                    e1 = e2;
                }
            }

            if (e1 != e2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return e1;
            }
        } else {
            if (!match_all) return e1;
            i_1 ++;
            if (i_1 < t1_count) {
                e1 = t1_array[i_1];
            }
        }
    }

    if (match_all) {
        return e1;
    } else {
        return 0;
    }
}

/* -- Public API -- */

int32_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == entity) {
            return c_ptr_i; 
        }
    });

    return -1;
}

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_get_stage(&world);
    
    ecs_table_t *table = ecs_table_from_type(world, type);
    ecs_entities_t add_array = ecs_type_to_entities(to_add);
    ecs_entities_t remove_array = ecs_type_to_entities(to_remove);
    
    table = ecs_table_traverse_remove(
        world, table, &remove_array, NULL); 

    table = ecs_table_traverse_add(
        world, table, &add_array, NULL); 

    if (!table) {
        return NULL;
    } else {
        return table->type;
    }
}

ecs_type_t ecs_type_find(
    ecs_world_t *world,
    ecs_entity_t *array,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_get_stage(&world);

    ecs_entities_t entities = {
        .array = array,
        .count = count
    };

    ecs_table_t *table = ecs_table_find_or_create(world, &entities);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

static
bool has_trait(
    ecs_entity_t trait,
    ecs_entity_t e)
{
    return trait == ecs_entity_t_hi(e & ECS_COMPONENT_MASK);
}

static
bool has_case(
    ecs_world_t *world,
    ecs_entity_t sw_case,
    ecs_entity_t e)
{
    const EcsType *type_ptr = ecs_get(world, e & ECS_COMPONENT_MASK, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_type_has_entity(world, type_ptr->normalized, sw_case);
}

static
int match_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e,
    ecs_entity_t match_with)
{
    if (ECS_HAS_ROLE(match_with, TRAIT)) {
        ecs_entity_t hi = ecs_entity_t_hi(match_with & ECS_COMPONENT_MASK);
        ecs_entity_t lo = ecs_entity_t_lo(match_with);

        if (lo == EcsWildcard) {
            ecs_assert(hi != 0, ECS_INTERNAL_ERROR, NULL);
            
            if (!ECS_HAS_ROLE(e, TRAIT) || !has_trait(hi, e)) {
                return 0;
            }

            ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
            int32_t i, count = ecs_vector_count(type);

            ecs_entity_t comp = ecs_entity_t_lo(e);
            for (i = 0; i < count; i ++) {
                if (comp == ids[i]) {
                    return 2;
                }
            }

            return -1;
        } else if (!hi) {
            if (ECS_HAS_ROLE(e, TRAIT) && has_trait(lo, e)) {
                return 1;
            }
        }
    } else 
    if (ECS_HAS_ROLE(match_with, CASE)) {
        ecs_entity_t sw_case = match_with & ECS_COMPONENT_MASK;
        if (ECS_HAS_ROLE(e, SWITCH) && has_case(world, sw_case, e)) {
            return 1;
        } else {
            return 0;
        }
    }

    if (e == match_with) {
        return 1;
    }

    return 0;
}

static
bool search_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    if (!type) {
        return false;
    }

    if (!entity) {
        return true;
    }

    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);
    int matched = 0;

    for (i = 0; i < count; i ++) {
        int ret = match_entity(world, type, ids[i], entity);
        switch(ret) {
        case 0: break;
        case 1: return true;
        case -1: return false;
        case 2: matched ++; break;
        default: ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (!matched && !owned && entity != EcsPrefab && entity != EcsDisabled) {
        for (i = count - 1; i >= 0; i --) {
            ecs_entity_t e = ids[i];
            if (!ECS_HAS_ROLE(e, INSTANCEOF)) {
                break;
            }

            ecs_entity_t base = e & ECS_COMPONENT_MASK;
            ecs_type_t base_type = ecs_get_type(world, base);

            if (search_type(world, base_type, entity, false)) {
                return true;
            }
        }
    }

    return matched != 0;
}

bool ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    return search_type(world, type, entity, false);
}

bool ecs_type_owns_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    return search_type(world, type, entity, owned);
}

bool ecs_type_has_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has)
{
    return ecs_type_contains(world, type, has, true, false) != 0;
}

bool ecs_type_owns_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return ecs_type_contains(world, type, has, true, !owned) != 0;
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_add(world, table, &entities, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_remove(world, table, &entities, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;    
}

char* ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;

    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        char buffer[256];
        ecs_size_t len;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (e == 1) {
            ecs_os_strcpy(buffer, "EcsComponent");
            len = ecs_os_strlen("EcsComponent");
        } else {
            len = ecs_from_size_t(ecs_entity_str(world, e, buffer, 256));
        }

        dst = ecs_vector_addn(&chbuf, char, len);
        ecs_os_memcpy(dst, buffer, len);
    }

    *(char*)ecs_vector_add(&chbuf, char) = '\0';

    char* result = ecs_os_strdup(ecs_vector_first(chbuf, char));
    ecs_vector_free(chbuf);
    return result;
}

ecs_entity_t ecs_type_get_entity_for_xor(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t xor)
{
    ecs_assert(
        ecs_type_owns_entity(world, type, ECS_XOR | xor, true),
        ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get(world, xor, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t xor_type = type_ptr->normalized;
    ecs_assert(xor_type != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        if (ecs_type_owns_entity(world, xor_type, array[i], true)) {
            return array[i];
        }
    }

    return 0;
}

int32_t ecs_type_trait_index_of(
    ecs_type_t type, 
    int32_t start_index, 
    ecs_entity_t trait)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    for (i = start_index; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (ECS_HAS_ROLE(e, TRAIT)) {
            e &= ECS_COMPONENT_MASK;
            if (trait == ecs_entity_t_hi(e)) {
                return i;
            }
        }
    }

    return -1;
}
