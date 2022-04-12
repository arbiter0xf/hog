#ifndef HOG_UTIL_DEFINED
#define HOG_UTIL_DEFINED

#include <stdint.h>

#define HEX_STR_SIZE_32 9 // 'f''f''f''f''f''f''f''f''\0'
#define HEX_STR_SIZE_64 17

uint64_t strlen(const char* str);
void uint32_to_hex_str(const uint32_t num, char* str);
void uint64_to_hex_str(const uint64_t num, char* str);

#endif
