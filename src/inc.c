#include "inc.h"

#include "arena.h"

// temp
Arena tmp;
void init_tmp() {
    // init tmp
    arena_create(&tmp, 1 * 1024 * 1024);
}

void shutdown_tmp() {
    // destroy tmp
    arena_destroy(&tmp);
}

void reset_tmp() {
    arena_reset(&tmp);
}

char* tprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char *result = arena_vsprintf(&tmp, format, args);
    va_end(args);

    return result;
}

void* talloc(size_t size, size_t align) {
    return arena_alloc(&tmp, size, align);
}

/* string string_span(u8 *beg, u8 *end) { */
/*     string s = {0}; */
/*     s.data = beg; */
/*     s.len = end - beg; */
/*     return s; */
/* } */

/* u64 string_hash(string s) { */
/*     u64 h = 0; */
/*     __builtin_memcpy(&h, s.data, min(8, s.len)); */
/*     return h * 1111111111111111111u; */
/* } */

/* b32 string_equals(string a, string b) { */
/*     return a.len==b.len && !__builtin_memcmp(a.data, b.data, a.len); */
/* } */

/* size string_compare(string a, string b) { */
/*     size len = a.len<b.len ? a.len : b.len; */
/*     size r = len ? __builtin_memcmp(a.data, b.data, len) : b.len-a.len; */
/*     return r ? r : a.len-b.len; */
/* } */
