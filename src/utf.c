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

#define num_cmp(i, j) 2 * ((i) > (j)) - 1

// Checks if two utf-8 characters are the same or not.
// -1 when c1 < c2
//  0 when c1 == c2
//  1 when c1 > c2
int tsm_rune_cmp(const char *c1, int size1, const char *c2, int size2) {
    if (size1 != size2) return num_cmp(size1, size2);
    for (int i = 0; i < size1; i++) {
        if (*c1 != *c2)
            return num_cmp((uint8_t)(*c1), (uint8_t)(*c2));
        c1++;
        c2++;
    }
    return 0;
}
