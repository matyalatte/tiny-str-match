#include "str_match.h"

#define ASCII_MAX 0x7F  // ascii 0x00 ~ 0x7F
#define MULTIBYTE_SEQ_MAX 0xBF  // sequences for multibyte characters 0x80 ~ 0xBF
#define TWO_BYTE_MAX 0xDF  // two-byte characters 0xC0 ~ 0xDF
#define THREE_BYTE_MAX 0xEF  // three-byte characters 0xE0 ~ 0xEF
#define FOUR_BYTE_MAX 0xF7  // four-byte characters 0xF0 ~ 0xF7
// unused codes 0xF8 ~ 0xFF
// #define FIVE_BYTE_MAX 0xFB
// #define SIX_BYTE_MAX 0xFD

#define is_multibyte_seq(c) ((ASCII_MAX < (uint8_t)(c)) && ((uint8_t)(c) <= MULTIBYTE_SEQ_MAX))

// Counts the binary size of an utf-8 character.
int tsmRuneSize(const char *c) {
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
