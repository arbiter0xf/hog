#include <stddef.h>
#include <stivale2.h>

#include <hog_util.h>

#define STACK_SIZE 8192

extern int is_cpuid_supported(void);

static uint8_t stack[STACK_SIZE];

// TODO comments
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

// TODO comments
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

// TODO comments
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, const uint64_t id)
{
    struct stivale2_tag* current_tag = (void*) stivale2_struct->tags;

    while (NULL != current_tag) {
        if (id == current_tag->identifier) {
            break;
        }

        current_tag = (void*) current_tag->next;
    }

    return current_tag;
}

void halt_execution(void)
{
    for (;;) {
        asm ("hlt");
    }
}

// TODO comments
// https://github.com/stivale/stivale/blob/master/STIVALE2.md#kernel-entry-machine-state
void _start(struct stivale2_struct* stivale2_struct)
{
    struct stivale2_struct_tag_terminal* tag_terminal = 0;
    void (*term_write)(const char* str, size_t len) = 0;
    void* terminal_write_ptr = 0;
    uint32_t cpuid_supported = 0;
    char cpuid_supported_hex_str[HEX_STR_SIZE_32] = {0};

    cpuid_supported = is_cpuid_supported();
    (void)cpuid_supported;

    uint32_to_hex_str(cpuid_supported, cpuid_supported_hex_str);

    tag_terminal = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    if (NULL == tag_terminal) {
        halt_execution();
    }

    terminal_write_ptr = (void*) tag_terminal->term_write;
    term_write = terminal_write_ptr;

    term_write("Terminal test print\n", 20);
    term_write("CPUID supported: ", 17);
    term_write(cpuid_supported_hex_str, 8);
    term_write("\n", 1);

    halt_execution();
}
