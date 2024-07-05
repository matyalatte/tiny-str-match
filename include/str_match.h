#ifndef __TINY_STR_MATCH_INCLUDE_STR_MATCH_H__
#define __TINY_STR_MATCH_INCLUDE_STR_MATCH_H__
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TSM_EXTERN
#ifdef _WIN32
#define _TSM_EXTERN __declspec(dllexport) extern
#else
#define _TSM_EXTERN __attribute__((visibility("default"))) extern
#endif
#endif

// Version info
#define TSM_VERSION "0.1.0"

/**
 * Checks if a string matches a wildcard pattern or not.
 *
 * @note "?" for any single character (including multi-byte characters.) "*" for any number of characters.
 *
 * @param pattern A wildcard pattern.
 * @param str A string.
 * @returns One when the string has the wildcard pattern. Zero if not.
 */
_TSM_EXTERN int tsm_wildcard_match(const char *pattern, const char *str);

/**
 * Checks if a string matches a regex pattern or not.
 *
 * @param pattern A regex pattern.
 * @param str A string.
 * @returns One when the string has the regex pattern. Zero if not.
 */
_TSM_EXTERN int tsm_regex_match(const char *pattern, const char *str);


#ifdef __cplusplus
}
#endif

#endif  // __TINY_STR_MATCH_INCLUDE_STR_MATCH_H__
