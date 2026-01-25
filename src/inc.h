#ifndef INC_H_
#define INC_H_

#include <stdint.h>
#include <stddef.h>

// primitives
typedef uint8_t   u8;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int32_t   i32;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;
typedef u8        b8;
typedef char      byte;
typedef uintptr_t uptr;
typedef ptrdiff_t size;
typedef size_t    usize;

// util
#define assert(c)      while (!(c)) __builtin_unreachable()
#define max(a, b)      ((a)>(b) ? (a) : (b))
#define min(a, b)      ((a)<(b) ? (a) : (b))
#define clamp(v, a, b) (min(max(a, v), b))
#define countof(a)     (size)(sizeof(a) / sizeof(a[0]))

// constants
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

// temp allocation
void init_tmp();
void shutdown_tmp();
void reset_tmp();
char* tprintf(const char* format, ...)
	__attribute__((format(printf, 1, 2)));
void* talloc(size_t size, size_t align);

// strings
/* #define s8(s) (s8){(u8 *)s, lengthof(s) - 1} */
/* typedef struct { */
/*     u8  *data; */
/*     size len; */
/* } string; */

/* typedef struct { */
/*     char* items; */
/*     size count; */
/*     size capacity; */
/* } type; */

/* string   string_span(u8 *, u8 *); */
/* b32      string_equals(string, string); */
/* size     string_compare(string, string); */
/* u64      string_hash(string); */


#endif
