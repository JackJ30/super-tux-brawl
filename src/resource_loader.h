#ifndef RESOURCE_LOADER_H_
#define RESOURCE_LOADER_H_

#include "inc.h"

char* get_resource_dir();
void load_resource_file(char* path, byte* out, size* size);

#endif
