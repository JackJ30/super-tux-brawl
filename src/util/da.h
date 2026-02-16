#ifndef DA_H_
#define DA_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "arena.h"

#define DA(type) type *

typedef struct {
	size_t size;
	size_t capacity;
    Arena* arena;
	char da[];
} da_info;

da_info* da_create_(size_t item_size, size_t size, Arena* arena);
void da_print(void* da);
void da_pop(void* da);
void* da_resize_(void* da, size_t item_size, size_t size);

#define CONCAT_INNER(a, b) a ## b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define da_clear(da) (((da_info*)(da))[-1].size = 0)
#define da_size(da) (((da_info*)(da))[-1].size)
#define da_create(type, size) ((type*)(da_create_(sizeof(type), size, NULL)->da))
#define arena_da_create(arena, type, size) ((type*)(da_create_(sizeof(type), size, arena)->da))

#define da_destroy(da) do {                             \
        da_info* info = &(((da_info*)(da))[-1]);        \
        if (!info->arena) free(((da_info*)(da)) - 1);   \
        (da) = NULL;                                    \
    } while (0)

#define da_erase(da, index) do {                                \
        for (int __i = (index); __i < da_size((da)) - 1; __i++) \
            (da)[__i] = (da)[__i + 1];                          \
        ((da_info*)(da))[-1].size--;                            \
    } while(0)

#define da_erase_range(da, startinc, endinc) do {                       \
        for (int __i = (startinc); __i < da_size((da)) - ((endinc) - (startinc) + 1); __i++) \
            (da)[__i] = (da)[__i + ((endinc) - (startinc) + 1)];        \
        ((da_info*)(da))[-1].size -= ((endinc) - (startinc) + 1);       \
    } while(0)

#define da_insert(da, index, value) do {                                \
        size_t __idx = (index);                                         \
        void* __temp = da_resize_((da), sizeof(*(da)), da_size(da) + 1); \
        (da) = __temp;                                                  \
        for (int __i = da_size((da)) - 1; __i > (__idx); __i--)         \
            (da)[__i] = (da)[__i - 1];                                  \
        (da)[(__idx)] = value;                                          \
    } while(0)

#define da_insert_many(da, index, values, len) do {                     \
        size_t __idx = (index);                                         \
        void* __temp = da_resize_((da), sizeof(*(da)), da_size(da) + len); \
        (da) = __temp;                                                  \
        for (int __i = da_size((da)) - 1; __i > (__idx) + len - 1; __i--) \
            (da)[__i] = (da)[__i - len];                                \
        memcpy(&da[(__idx)], (values), len * sizeof(*(da)));            \
    } while (0)

#define da_insert_str(da, index, values) da_insert_many((da), (index), (values), strlen(values))

#define da_add_many(da, values, len) do {                               \
        void* __temp = da_resize_((da), sizeof(*(da)), da_size(da) + (len)); \
        (da) = __temp;                                                  \
        memcpy(&(da[da_size((da)) - (len)]), (values), (len) * sizeof(*(da))); \
    } while (0)

#define da_add_str(da, values) da_add_many((da), (values), strlen(values))

#define da_resize(da, size) do {                                \
        void* __temp = da_resize_((da), sizeof(*(da)), size);   \
        (da) = __temp;                                          \
    } while (0)

#define da_add(da, value) do {                              \
        void* __temp = da_resize_((da), sizeof(*(da)), -1); \
        (da) = __temp;                                      \
        (da)[da_size((da)) - 1] = value;                    \
    } while (0)

#define da_last(da) (da)[da_size(da) - 1]

#define da_for_all(type__, name__, da__)                                \
    type__* CONCAT(it__, __LINE__) = da__; type__* CONCAT(end__, __LINE__) = da__ ? da__ + da_size(da__) : NULL; \
    for (type__* name__; (CONCAT(it__, __LINE__) < CONCAT(end__, __LINE__)) ? (name__ = CONCAT(it__, __LINE__), true) : false; CONCAT(it__, __LINE__)++)

#define da_dump(da, type, name, fmt,  ...) do {                         \
        da_print(da);                                                   \
        fprintf(stderr, "vals: {\n");                                   \
        type* it__  = (da); type* end__ = (da) ? (da) + da_size(da) : NULL; \
        for (type* name; it__ < end__ ? (name = it__, true) : false; it__++) { \
            fprintf(stderr, "    "fmt",\n", __VA_ARGS__);               \
        }                                                               \
        fprintf(stderr, "}\n");                                         \
    } while (0)

#endif // DA_H_
