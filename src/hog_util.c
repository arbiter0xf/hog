#include <hog_util.h>

uint64_t strlen(const char* str)
{
    uint64_t i = 0;

    for (i = 0; i < UINT64_MAX; i++) {
        if ('\0' == str[i]) {
            break;
        }
    }

    return i;
}

void uint64_to_hex_str(const uint64_t num, char* str)
{
    uint64_t mask = 0xf;
    uint8_t shift = 60;
    uint8_t i = 0;

    mask = mask << 60;
    for (i = 0; i < HEX_STR_SIZE_64 - 1; i++) {
        str[i] = (char) ((num & mask) >> shift);
        mask = mask >> 4;
        shift = shift - 4;
    }

    for (i = 0; i < HEX_STR_SIZE_64 - 1; i++) {
        if (str[i] < 10) {
            str[i] += '0';
        } else {
            str[i] = 'a' + str[i] - 10;
        }
    }

    str[i] = '\0';
}

void uint32_to_hex_str(const uint32_t num, char* str)
{
    uint8_t i = 0;

    str[0] = (char) ((num & 0xf0000000) >> 28);
    str[1] = (char) ((num & 0x0f000000) >> 24);
    str[2] = (char) ((num & 0x00f00000) >> 20);
    str[3] = (char) ((num & 0x000f0000) >> 16);
    str[4] = (char) ((num & 0x0000f000) >> 12);
    str[5] = (char) ((num & 0x00000f00) >> 8);
    str[6] = (char) ((num & 0x000000f0) >> 4);
    str[7] = (char) ((num & 0x0000000f) >> 0);

    for (i = 0; i < HEX_STR_SIZE_32 - 1; i++) {
        if (str[i] < 10) {
            str[i] += '0';
        } else {
            str[i] = 'a' + str[i] - 10;
        }
    }

    str[i] = '\0';
}
