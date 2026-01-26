#ifndef DA_H_
#define DA_H_

#define ARENA_DA_INIT_CAP 256

#define DA(type) struct {                       \
        size_t count;                           \
        size_t capacity;                        \
        type *items;                            \
    }

#define DA_DEFINE(type, name) typedef DA(type) name

#define arena_da_create(a, da, size) do {                               \
        (da)->count = size;                                             \
        (da)->capacity = size;                                          \
        (da)->items = arena_alloc((a), (da)->capacity*sizeof(*((da)->items)), _Alignof(*((da)->items))); \
    } while (0)

#define arena_da_append(a, da, item) do {                               \
        if ((da)->count >= (da)->capacity) {							\
            usize new_capacity = (da)->capacity == 0 ? ARENA_DA_INIT_CAP : (da)->capacity*2; \
            (da)->items = arena_realloc((a), (da)->items, (da)->capacity*sizeof(*((da)->items)), new_capacity*sizeof(*((da)->items)), _Alignof(*((da)->items))); \
            (da)->capacity = new_capacity;								\
        }																\
                                                                        \
        (da)->items[(da)->count++] = (item);							\
    } while (0)

// Append several items to a dynamic array
#define arena_da_append_many(a, da, new_items, new_items_count) do {    \
        if ((da)->count + (new_items_count) > (da)->capacity) {         \
            usize new_capacity = (da)->capacity;                        \
            if (new_capacity == 0) new_capacity = ARENA_DA_INIT_CAP;    \
            while ((da)->count + (new_items_count) > new_capacity) new_capacity *= 2; \
            (da)->items = arena_realloc((a), (da)->items, (da)->capacity*sizeof(*((da)->items)), new_capacity*sizeof(*(da)->items), _Alignof(*((da)->items))); \
            (da)->capacity = new_capacity;                              \
        }                                                               \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count)*sizeof(*((da)->items))); \
        (da)->count += (new_items_count);                               \
    } while (0)

#define da_create(da, size) do {                                        \
        (da)->count = size;                                             \
        (da)->capacity = size;                                          \
        (da)->items = malloc((da)->capacity*sizeof(*((da)->items)));\
    } while (0)

#define da_destroy(da) do {                                             \
        free((da)->items);                                              \
    } while (0)

#define da_append(da, item) do {                                        \
        if ((da)->count >= (da)->capacity) {							\
            usize new_capacity = (da)->capacity == 0 ? ARENA_DA_INIT_CAP : (da)->capacity*2; \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*((da)->items)), new_capacity*sizeof(*((da)->items))); \
            (da)->capacity = new_capacity;								\
        }																\
                                                                        \
        (da)->items[(da)->count++] = (item);							\
    } while (0)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count) do {             \
        if ((da)->count + (new_items_count) > (da)->capacity) {         \
            usize new_capacity = (da)->capacity;                        \
            if (new_capacity == 0) new_capacity = ARENA_DA_INIT_CAP;    \
            while ((da)->count + (new_items_count) > new_capacity) new_capacity *= 2; \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*((da)->items)), new_capacity*sizeof(*(da)->items)); \
            (da)->capacity = new_capacity;                              \
        }                                                               \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count)*sizeof(*((da)->items))); \
        (da)->count += (new_items_count);                               \
    } while (0)

#endif // DA_H_
