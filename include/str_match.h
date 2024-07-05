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

#define _TSM_ENUM(s) typedef int s; enum

// Version info
#define TSM_VERSION "0.1.0"

/**
 * Error status for tsm_* functions.
 *
 * @enum TsmResult
 */
_TSM_ENUM(TsmResult) {
    TSM_OK = 0,
    TSM_FAIL = 1,
    TSM_SYNTAX_ERROR = 2,
};

/**
 * Checks if a string matches a wildcard pattern or not.
 *
 * @note "?" for any single character (including multi-byte characters.) "*" for any number of characters.
 *
 * @param pattern A wildcard pattern.
 * @param str A string.
 * @returns Zero when the string has the wildcard pattern. One if not.
 */
_TSM_EXTERN TsmResult tsm_wildcard_match(const char *pattern, const char *str);

/**
 * Checks if a string matches a regex pattern or not.
 *
 * @param pattern A regex pattern.
 * @param str A string.
 * @returns Zero when found the regex pattern. One when not found. Two when got a syntax error.
 */
_TSM_EXTERN TsmResult tsm_regex_match(const char *pattern, const char *str);


#ifdef __cplusplus
}
#endif

#endif  // __TINY_STR_MATCH_INCLUDE_STR_MATCH_H__
