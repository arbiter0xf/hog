#include <stddef.h>
#include <stivale2.h>

#include <hog_stivale2.h>
#include <hog_util.h>

#define RET_OK 0
#define RET_ERR -1

#define PS2_CONTROLLER_TEST_RESPONSE_PASS 0x55
#define PS2_CONTROLLER_TEST_RESPONSE_FAIL 0xFC
#define PS2_PORT_TEST_RESPONSE_PASS 0
#define PS2_TEST_PASS 0
#define PS2_TEST_FAIL 1
#define PS2_TEST_RUN_UNSUCCESSFUL 2

extern int is_cpuid_supported(void);
extern void usb_legacy_ps2_controller_write_control_byte_aa(void);
extern uint8_t usb_legacy_ps2_controller_read_response_byte(void);
extern void usb_legacy_ps2_controller_enable_first_port(void);
extern void usb_legacy_ps2_controller_write_control_byte_ab(void);
extern uint32_t get_register_64_value_cr3(void);

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

static uint8_t test_ps2_controller(void)
{
    uint8_t response = -1;

    term_write("Testing PS/2 controller...", 27);

    usb_legacy_ps2_controller_write_control_byte_aa();
    response = usb_legacy_ps2_controller_read_response_byte();

    if (PS2_CONTROLLER_TEST_RESPONSE_PASS == response) {
        term_write(": TEST PASS\n", 12);
        return PS2_TEST_PASS;
    }

    if (PS2_CONTROLLER_TEST_RESPONSE_FAIL == response) {
        term_write(": TEST FAIL\n", 12);
        return PS2_TEST_FAIL;
    }

    term_write(": TEST RUN UNSUCCESSFUL\n", 24);
    return PS2_TEST_RUN_UNSUCCESSFUL;
}

static void ps2_controller_enable_first_port(void)
{
    term_write("Enabling PS/2 first port\n", 25);
    usb_legacy_ps2_controller_enable_first_port();
}

/*
 * Test first PS/2 port
 * Control byte for running test: 0xAB
 * Return values:
 *       -> 0x00 test passed
 *       -> 0x01 clock line stuck low
 *       -> 0x02 clock line stuck high
 *       -> 0x03 data line stuck low
 *       -> 0x04 data line stuck high
 */
static uint8_t ps2_test_first_port(void) {
    uint8_t response = -1;
    char response_hex_str[HEX_STR_SIZE_32];

    term_write("Testing PS/2 first port...", 26);

    usb_legacy_ps2_controller_write_control_byte_ab();
    response = usb_legacy_ps2_controller_read_response_byte();
    uint32_to_hex_str(response, response_hex_str);

    if (PS2_PORT_TEST_RESPONSE_PASS == response) {
        term_write(": TEST PASS\n", 12);
        return PS2_TEST_PASS;
    }

    term_write(": TEST FAIL: ", 13);
    term_write(response_hex_str, 8);
    term_write("\n", 1);
    return PS2_TEST_FAIL;
}

static uint8_t ps2_controller_initialize(void)
{
    uint8_t test_result = -1;
    term_write("Initializing PS/2\n", 18);

    test_result = test_ps2_controller();
    if (PS2_TEST_PASS != test_result) {
        return RET_ERR;
    }

    ps2_controller_enable_first_port();
    ps2_test_first_port();

    // TODO disable second port
    // term_write("", 18);

    return RET_OK;
}

static void read_ps2_response_hex(char* hex_str)
{
    uint8_t response = -1;

    response = usb_legacy_ps2_controller_read_response_byte();
    uint32_to_hex_str(response, hex_str);
}

static void busyloop()
{
    const uint32_t busyloop_max = 250000000;

    // Use volatile for preventing compiler optimizing the loop away.
    for (volatile uint32_t i = 0; i < busyloop_max; i++) { }
}

static void dump_registers(void)
{
    uint64_t cr3_value = get_register_64_value_cr3();
    char cr3_value_hex_str[HEX_STR_SIZE_64] = {0};
    uint64_to_hex_str(cr3_value, cr3_value_hex_str);
    term_write("Printing register values\n", 25);
    term_write("CR3: ", 5);
    term_write(cr3_value_hex_str, 16);
    term_write("\n", 1);
}

static void dump_paging(struct stivale2_struct_tag_hhdm* tag_hhdm)
{
    uint64_t cr3_value = get_register_64_value_cr3();
    uint64_t* pml4_base_virtual = (uint64_t*)
        (tag_hhdm->addr +
        (cr3_value & ~0xfff));
    char pml4_base_virtual_hex_str[HEX_STR_SIZE_64] = {0};
    char pml4_entry_address_hex_str[HEX_STR_SIZE_64] = {0};
    char pml4_entry_value_hex_str[HEX_STR_SIZE_64] = {0};

    uint64_to_hex_str(
            (uint64_t) pml4_base_virtual,
            pml4_base_virtual_hex_str);
    term_write("PML4 base (virtual): ", 21);
    term_write(pml4_base_virtual_hex_str, 16);
    term_write("\n", 1);

    term_write("Initial non-zero PML4 entries: \n", 32);
    const uint16_t entries_max = 512;
    for (uint16_t i = 0; i < entries_max; i++) {
        const uint64_t* pml4_entry_address = pml4_base_virtual + i;

        if (0 != *pml4_entry_address) {
            uint64_to_hex_str((uint64_t) pml4_entry_address, pml4_entry_address_hex_str);
            uint64_to_hex_str(*pml4_entry_address, pml4_entry_value_hex_str);

            term_write(pml4_entry_address_hex_str, 16);
            term_write(": ", 2);
            term_write(pml4_entry_value_hex_str, 16);
            term_write("\n", 1);
        }
    }
}

// TODO comments
// https://github.com/stivale/stivale/blob/master/STIVALE2.md#kernel-entry-machine-state
void _start(struct stivale2_struct* stivale2_struct)
{
    uint8_t ret = RET_ERR;
    uint8_t ps2_init_ok = 0;
    char ps2_input_hex_str[HEX_STR_SIZE_32] = {0};
    char hhdm_address_hex_str[HEX_STR_SIZE_64] = {0};
    struct stivale2_struct_tag_hhdm* tag_hhdm = 0;
    initialize_terminal_printing(stivale2_struct);
    term_write("Terminal test print\n", 20);

    detect_cpuid_support();

    ret = ps2_controller_initialize();
    if (RET_OK == ret) {
        term_write("PS/2 successfully initialized\n", 30);
        ps2_init_ok = 1;
    }

    tag_hhdm = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_HHDM_ID);
    if (NULL == tag_hhdm) {
        term_write("Failed to get HHDM tag. Halting...\n", 35);
        halt_execution();
    }

    dump_registers();

    uint64_to_hex_str(tag_hhdm->addr, hhdm_address_hex_str);
    term_write("HHDM address: ", 14);
    term_write(hhdm_address_hex_str, 16);
    term_write("\n", 1);

    dump_paging(tag_hhdm);

    if (ps2_init_ok) {
        term_write("Polling for PS/2 input...\n", 26);

        for (;;) {
            busyloop();

            // Try to read and display data sent by PS/2 device
            read_ps2_response_hex(ps2_input_hex_str);
            term_write(ps2_input_hex_str, 8);
            term_write("\n", 1);
        }
    }

    term_write("Halting execution\n", 18);
    halt_execution();
}
