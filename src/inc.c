#include "inc.h"

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
