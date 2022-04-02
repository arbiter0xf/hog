#include <hog_util.h>

void uint32_to_hex_str(const uint32_t num, char* str)
{
    str[0] = (char) ((num & 0xf0000000) >> 28);
    str[1] = (char) ((num & 0x0f000000) >> 24);
    str[2] = (char) ((num & 0x00f00000) >> 20);
    str[3] = (char) ((num & 0x000f0000) >> 16);
    str[4] = (char) ((num & 0x0000f000) >> 12);
    str[5] = (char) ((num & 0x00000f00) >> 8);
    str[6] = (char) ((num & 0x000000f0) >> 4);
    str[7] = (char) ((num & 0x0000000f) >> 0);

    for (unsigned int i = 0; i < HEX_STR_SIZE_32; i++) {
        if (str[i] < 10) {
            str[i] += '0';
        } else {
            str[i] = 'a' + str[i] - 10;
        }
    }
}
