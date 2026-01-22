#include "resource_loader.h"

char* get_resource_dir() {
#if DEBUG
    return "out-debug/";
#else
    // todo(jack): eventually check install location
    return "out-release/";
#endif
}

void load_resource_file(char* path, byte* out, size* size) {
    // todo(jack): implement
    assert(false);
}
