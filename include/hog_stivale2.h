#ifndef HOG_STIVALE2_DEFINED
#define HOG_STIVALE2_DEFINED

#include <stddef.h>
#include <stivale2.h>

extern struct stivale2_header_tag_terminal terminal_hdr_tag;
extern struct stivale2_header_tag_framebuffer framebuffer_hdr_tag;
extern struct stivale2_header stivale_hdr;

void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, const uint64_t id);

#endif
