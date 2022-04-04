#include <stddef.h>
#include <stivale2.h>

#include <hog_stivale2.h>
#include <hog_util.h>

extern int is_cpuid_supported(void);

static void (*term_write)(const char* str, size_t len) = 0;

static void halt_execution(void)
{
    for (;;) {
        asm ("hlt");
    }
}

static void (*get_term_write_function(struct stivale2_struct* stivale2_struct))(const char* str, size_t len)
{
    struct stivale2_struct_tag_terminal* tag_terminal = 0;
    void* terminal_write_ptr = 0;

    tag_terminal = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    if (NULL == tag_terminal) {
        halt_execution();
    }

    terminal_write_ptr = (void*) tag_terminal->term_write;

    return terminal_write_ptr;
}

static void detect_cpuid_support()
{
    uint32_t cpuid_supported = 0;
    char cpuid_supported_hex_str[HEX_STR_SIZE_32] = {0};

    term_write("Detecting CPUID support\n", 24);
    cpuid_supported = is_cpuid_supported();
    uint32_to_hex_str(cpuid_supported, cpuid_supported_hex_str);
    term_write("CPUID supported: ", 17);
    term_write(cpuid_supported_hex_str, 8);
    term_write("\n", 1);
}

static void initialize_terminal_printing(struct stivale2_struct* stivale2_struct)
{
    term_write = get_term_write_function(stivale2_struct);
}

// TODO comments
// https://github.com/stivale/stivale/blob/master/STIVALE2.md#kernel-entry-machine-state
void _start(struct stivale2_struct* stivale2_struct)
{
    initialize_terminal_printing(stivale2_struct);
    term_write("Terminal test print\n", 20);

    detect_cpuid_support();

    halt_execution();
}
