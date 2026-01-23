#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_

#include "inc.h"
#include "arena.h"

char* get_resource_dir();
b8 load_resource_file(char* path, Arena* arena, byte* out, size* size);

#endif
