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
#include "utf.h"

static int wildcard_match_base(const char* pattern, const char* str) {
    while (*pattern != '\0') {
        // count the binary size of each character.
        int p_rs = tsm_rune_size(pattern);
        int s_rs = tsm_rune_size(str);
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
        } else if (!tsm_rune_cmp(pattern, str, p_rs)) {
            return 0;
        }
        pattern += p_rs;
        str += s_rs;
    }
    return !*str;
}

int tsm_wildcard_match(const char *pattern, const char *str) {
    if (pattern == NULL || str == NULL)
        return 0;
    return wildcard_match_base(pattern, str);
}
