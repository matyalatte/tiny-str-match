#include "str_match.h"
#include "utf.h"

// Counts the binary size of an utf-8 character.
int tsm_rune_size(const char *c) {
    const uint8_t first = *c;
    if (first <= ASCII_MAX)
        return 1;  // ascii
    if (first <= MULTIBYTE_SEQ_MAX)
        return 0;  // bad rune
    if (first <= TWO_BYTE_MAX) {
        if (is_multibyte_seq(*(c + 1)))
            return 2;  // two-byte
        else
            return 0;  // bad rune
    }
    if (first <= THREE_BYTE_MAX) {
        if (is_multibyte_seq(*(c + 1))
            && is_multibyte_seq(*(c + 2)))
            return 3;  // three-byte
        else
            return 0;  // bad rune
    }
    if (first <= FOUR_BYTE_MAX) {
        if (is_multibyte_seq(*(c + 1))
            && is_multibyte_seq(*(c + 2))
            && is_multibyte_seq(*(c + 3)))
            return 4;  // four-byte
    }
    return 0;  // bad rune
}

uint32_t tsm_rune_code(const char *c, int rune_size) {
    uint32_t code = 0;
    for (int i = rune_size - 1; i >= 0; i--) {
        code += (uint32_t)((unsigned char)*c) << (8 * i);
        c++;
    }
    return code;
}

int tsm_rune_cmp(const char *c1, const char *c2, int rune_size) {
    for (int i = 0; i < rune_size; i++) {
        if (*c1 != *c2)
            return 0;
        c1++;
        c2++;
    }
    return 1;
}
