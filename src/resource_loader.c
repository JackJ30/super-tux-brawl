#include "resource_loader.h"

char* get_resource_dir() {
#if DEBUG
    return "out-debug/";
#else
    // todo(jack): eventually check install location
    return "out-release/";
#endif
}

b8 load_resource_file(char* path, Arena* arena, byte* out, size* size) {
    // todo(jack): implement
    assert(false);
}
