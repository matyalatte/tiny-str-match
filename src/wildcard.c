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

static TsmResult wildcard_match_base(const char* pattern, const char* str) {
    while (*pattern != '\0') {
        // count the binary size of each character.
        int p_rs = tsm_rune_size(pattern);
        int s_rs = tsm_rune_size(str);
        if (!p_rs || !s_rs)
            return TSM_FAIL;  // failed to parse utf-8 characters.

        if (*pattern == '*') {
            if (wildcard_match_base(pattern + p_rs, str) == TSM_OK)
                return TSM_OK;
            else if (!*str)
                return TSM_FAIL;
            str += s_rs;
            continue;
        }
        if (*pattern == '?') {
            if (!*str)
                return TSM_FAIL;
        } else if (tsm_rune_cmp(pattern, p_rs, str, s_rs)) {
            return TSM_FAIL;
        }
        pattern += p_rs;
        str += s_rs;
    }
    return (*str == 0 ? TSM_OK : TSM_FAIL);
}

TsmResult tsm_wildcard_match(const char *pattern, const char *str) {
    if (pattern == NULL || str == NULL)
        return TSM_FAIL;
    return wildcard_match_base(pattern, str);
}
