#ifndef __TINY_STR_MATCH_INCLUDE_UTF_H__
#define __TINY_STR_MATCH_INCLUDE_UTF_H__

#include <stdint.h>

#define ASCII_MAX 0x7F  // ascii 0x00 ~ 0x7F
#define MULTIBYTE_SEQ_MAX 0xBF  // sequences for multibyte characters 0x80 ~ 0xBF
#define TWO_BYTE_MAX 0xDF  // two-byte characters 0xC0 ~ 0xDF
#define THREE_BYTE_MAX 0xEF  // three-byte characters 0xE0 ~ 0xEF
#define FOUR_BYTE_MAX 0xF7  // four-byte characters 0xF0 ~ 0xF7
// unused codes 0xF8 ~ 0xFF
// #define FIVE_BYTE_MAX 0xFB
// #define SIX_BYTE_MAX 0xFD

#define is_multibyte_seq(c) ((ASCII_MAX < (uint8_t)(c)) && ((uint8_t)(c) <= MULTIBYTE_SEQ_MAX))

#ifdef __cplusplus
extern "C" {
#endif

extern int tsm_rune_size(const char *c);
extern uint32_t tsm_rune_code(const char *c, int rune_size);
extern int tsm_rune_cmp(const char *c1, const char *c2, int rune_size);

#ifdef __cplusplus
}
#endif

#endif  // __TINY_STR_MATCH_INCLUDE_UTF_H__
