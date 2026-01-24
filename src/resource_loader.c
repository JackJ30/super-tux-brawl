#include "resource_loader.h"
#include "logger.h"

#include <stdio.h>

char* get_resource_dir() {
#if DEBUG
    return "out-debug/";
#else
    // todo(jack): eventually check install location
    return "out-release/";
#endif
}

b8 load_resource_file(char* resource, Arena* arena, byte** data, size* size) {
	char* file_path = tprintf("%s%s", get_resource_dir(), resource);

    // file bullshit
	FILE* file = fopen(file_path, "rb");

	if (file == NULL) {
		log_err("Could not find resource: %s", resource);
		return false;
    }

	// determine size
	if (fseek(file, 0, SEEK_END) != 0) {
		log_err("Failed to seek in resource: %s", resource);
        goto err;
	}
    long int told_size = ftell(file);
	if (told_size < 0) {
		log_err("Failed to get size of resource: %s", resource);
        goto err;
	}
	*size = (usize)told_size;
	if (fseek(file, 0, SEEK_SET) != 0) {
		log_err("Failed to seek in resource: %s", resource);
        goto err;
	}

	// empty file
	if (*size == 0) {
		*data = NULL;
		log_warn("Resource %s is empty", resource);
		fclose(file);
		return true;
	}

	// allocate memory
	*data = arena_alloc(arena, *size, 1);
	if (!*data) {
		log_err("Failed to allocate memory for resource: %s", resource);
        goto err;
	}

	// read file
	size_t bytes_read = fread(*data, 1, *size, file);
	if (bytes_read != *size) {
		log_err("Failed to load resource: %s", resource);
        goto err;
	}

end:
	// clean up
	fclose(file);
	return true;

err:
    fclose(file);
    return false;
}
