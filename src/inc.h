#ifndef INC_H_
#define INC_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// primitives
typedef uint8_t   u8;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int32_t   i32;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef char      byte;
typedef uintptr_t uptr;
typedef ptrdiff_t size;
typedef size_t    usize;

// util
#define assert(c)      while (!(c)) __builtin_unreachable()
#define max(a, b)      ((a)>(b) ? (a) : (b))
#define min(a, b)      ((a)<(b) ? (a) : (b))
#define clamp(v, a, b) (min(max(a, v), b))
#define countof(a)     (size)(sizeof(a) / sizeof(*(a)))

// strings
/* #define s8(s) (s8){(u8 *)s, lengthof(s) - 1} */
/* typedef struct { */
/*     u8  *data; */
/*     size len; */
/* } string; */

/* string   string_span(u8 *, u8 *); */
/* b32      string_equals(string, string); */
/* size     string_compare(string, string); */
/* u64      string_hash(string); */

#endif
