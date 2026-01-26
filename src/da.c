#include <string.h>
#include "da.h"

da_info* da_create_(size_t item_size, size_t size, Arena* arena) {
	// find capacity
	size_t cap = 8;
	if (size > cap) {
		cap = size;

		// round up to next power of 2
		cap--;
		cap |= cap >> 1;
		cap |= cap >> 2;
		cap |= cap >> 4;
		cap |= cap >> 8;
		cap |= cap >> 16;
		cap++;
	}

	// make da
    da_info* info;
    if (arena) {
        info = arena_alloc_zero(arena, sizeof(da_info) + item_size * cap, _Alignof(da_info));
    }
    else {
        info = calloc(1, sizeof(da_info) + item_size * cap);
    }
	info->size = size;
	info->arena = arena;
	info->capacity = cap;
	return info;
}

void* da_resize_(void* da, size_t item_size, size_t size) {
	da_info* info = NULL;
	if (!da) {
		info = da_create_(item_size, 0, NULL);
	}
	else {
		info = ((da_info*)da) - 1;
	}
	if (size == -1) {
		info->size++;
	}
	else {
		info->size = size;
	}
	if (info->size >= info->capacity) {
        size_t oldsz = sizeof(array_info) + (info->capacity * item_size);
		info->capacity = info->size;
		info->capacity--;
		info->capacity |= info->capacity >> 1;
		info->capacity |= info->capacity >> 2;
		info->capacity |= info->capacity >> 4;
		info->capacity |= info->capacity >> 8;
		info->capacity |= info->capacity >> 16;
		info->capacity++;
        if (info->arena) {
            info = arena_realloc(info->arena, info, oldsz, sizeof(da_info) + (info->capacity * item_size), _Alignof(da_info));
        }
        else {
            info = realloc(info, sizeof(da_info) + (info->capacity * item_size));
        }
	}
	return info->da;
}

void da_print(void* da) {
	da_info* info = &((da_info*)da)[-1];
	fprintf(stderr,
            "size: %lu\n"
            "capacity: %lu\n"
            "da: %p\n",
            info->size,
            info->capacity,
            info->da);
}

void da_pop(void* da) {
	da_info* info = &((da_info*)da)[-1];
	if (info->size)
		info->size--;
}
