/*
 * Mini wildcard-module inspired by tinymatch.c:
 * https://github.com/r-lyeh/tinybits/blob/master/tinymatch.c
 *
 * Supports:
 * ---------
 *   '*'        Asterisk, matches zero or more
 *   '?'        Question, matches any character.
 *
 */

#include "str_match.h"

static int bufcmp(const char *str1, const char *str2, int len) {
    for (int i = 0; i < len; i++) {
        if (*str1 != *str2)
            return 0;
        str1++;
        str2++;
    }
    return 1;
}

static int wildcard_match_base(const char* pattern, const char* str) {
    while (*pattern != '\0') {
        // count the binary size of each character.
        int p_rs = tsmRuneSize(pattern);
        int s_rs = tsmRuneSize(str);
        if (!p_rs || !s_rs)
            return 0;  // failed to parse utf-8 characters.

        if (*pattern == '*') {
            if (wildcard_match_base(pattern + p_rs, str))
                return 1;
            else if (!*str)
                return 0;
            str += s_rs;
            continue;
        }
        if (*pattern == '?') {
            if (!*str)
                return 0;
        } else if (!bufcmp(pattern, str, p_rs)) {
            return 0;
        }
        pattern += p_rs;
        str += s_rs;
    }
    return !*str;
}

int tsmWildcardMatch(const char *pattern, const char *str) {
    if (pattern == NULL || str == NULL)
        return 0;
    return wildcard_match_base(pattern, str);
}
