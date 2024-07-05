/*
 * Mini regex-module inspired by tiny-regex-c:
 * https://github.com/kokke/tiny-regex-c/blob/master/re.c
 *
 * Supports:
 * ---------
 *   '.'        Dot, matches any character
 *   '^'        Start anchor, matches beginning of string
 *   '$'        End anchor, matches end of string
 *   '*'        Asterisk, match zero or more (greedy)
 *   '+'        Plus, match one or more (greedy)
 *   '?'        Question, match zero or one (non-greedy)
 *   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
 *   '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'} -- NOTE: feature is currently broken!
 *   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
 *   '\s'       Whitespace, \t \f \r \n \v and spaces
 *   '\S'       Non-whitespace
 *   '\w'       Alphanumeric, [a-zA-Z0-9_]
 *   '\W'       Non-alphanumeric
 *   '\d'       Digits, [0-9]
 *   '\D'       Non-digits
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "str_match.h"
#include "utf.h"
#include "re.h"


/* Definitions: */
#define MAX_REGEXP_OBJECTS      30    /* Max number of regex symbols in expression. */
#define MAX_CHAR_CLASS_LEN      40    /* Max length of character-class buffer in.   */

enum {
    UNUSED, DOT, BEGIN, END, QUESTIONMARK, STAR, PLUS,
    CHAR, CHAR_CLASS, INV_CHAR_CLASS, DIGIT, NOT_DIGIT,
    ALPHA, NOT_ALPHA, WHITESPACE, NOT_WHITESPACE, /* BRANCH */
};

typedef struct regex_t {
    unsigned char  type;   /* CHAR, STAR, etc.                      */
    union {
        unsigned char  ch[4];   /*      the character itself             */
        unsigned char* ccl;  /*  OR  a pointer to characters in class */
    } u;
    int ch_size;
} regex_t;


/* Private function declarations: */
static int matchpattern(regex_t* pattern, const char* text, int rune_size, int* matchlength);
static int matchcharclass(const char* c, int c_size, const char* str);
static int matchstar(regex_t p, regex_t* pattern,
                     const char* text, int rune_size, int* matchlength);
static int matchplus(regex_t p, regex_t* pattern,
                     const char* text, int rune_size, int* matchlength);
static int matchone(regex_t p, const char* c, int rune_size);
static int matchdigit(char c);
static int matchalpha(char c);
static int matchwhitespace(char c);
static int matchmetachar(const char* c, int c_size, const char* str, int rune_size);
static int matchrange(const char* c, int c_size, const char* str, int rune_size);
static int matchdot(char c);
static int ismetachar(char c);


/* Public functions: */
int re_match(const char* pattern, const char* text, int* matchlength) {
    return re_matchp(re_compile(pattern), text, matchlength);
}

int re_matchp(re_t pattern, const char* text, int* matchlength) {
    *matchlength = 0;
    int rune_size = tsm_rune_size(text);
    if (!rune_size) return -1;
    if (pattern != 0) {
        if (pattern[0].type == BEGIN) {
            return ((matchpattern(&pattern[1], text, rune_size, matchlength)) ? 0 : -1);
        } else {
            int idx = -1;

            do {
                idx += 1;

                if (matchpattern(pattern, text, rune_size, matchlength)) {
                    if (text[0] == '\0')
                        return -1;

                    return idx;
                }
                //  Reset match length for the next starting point
                *matchlength = 0;
                if (*text == '\0') break;
                text += rune_size;
                rune_size = tsm_rune_size(text);
                if (!rune_size) break;
            } while (1);
        }
    }
    return -1;
}

re_t re_compile(const char* pattern) {
    /* The sizes of the two static arrays below substantiates the static RAM usage of this module.
        MAX_REGEXP_OBJECTS is the max number of symbols in the expression.
        MAX_CHAR_CLASS_LEN determines the size of buffer for chars in all char-classes in the expression. */
    static regex_t re_compiled[MAX_REGEXP_OBJECTS];
    static unsigned char ccl_buf[MAX_CHAR_CLASS_LEN];
    int ccl_bufidx = 1;

    char c;     /* current char in pattern   */
    int c_size;
    int i = 0;  /* index into pattern        */
    int j = 0;  /* index into re_compiled    */

    while (pattern[i] != '\0' && (j + 1 < MAX_REGEXP_OBJECTS)) {
        c = pattern[i];
        c_size = tsm_rune_size(&pattern[i]);
        if (!c_size) return 0;  // failed to parse UTF-8 character.
        switch (c) {
        /* Meta-characters: */
        case '^': {    re_compiled[j].type = BEGIN;           } break;
        case '$': {    re_compiled[j].type = END;             } break;
        case '.': {    re_compiled[j].type = DOT;             } break;
        case '*': {    re_compiled[j].type = STAR;            } break;
        case '+': {    re_compiled[j].type = PLUS;            } break;
        case '?': {    re_compiled[j].type = QUESTIONMARK;    } break;
    /*  case '|': {    re_compiled[j].type = BRANCH;          } break; <-- not working properly */

        /* Escaped character-classes (\s \w ...): */
        case '\\':
        {
            if (pattern[i + 1] != '\0') {
                /* Skip the escape-char '\\' */
                i += 1;
                /* ... and check the next */
                switch (pattern[i]) {
                    /* Meta-character: */
                    case 'd': {    re_compiled[j].type = DIGIT;            } break;
                    case 'D': {    re_compiled[j].type = NOT_DIGIT;        } break;
                    case 'w': {    re_compiled[j].type = ALPHA;            } break;
                    case 'W': {    re_compiled[j].type = NOT_ALPHA;        } break;
                    case 's': {    re_compiled[j].type = WHITESPACE;       } break;
                    case 'S': {    re_compiled[j].type = NOT_WHITESPACE;   } break;

                    /* Escaped character, e.g. '.' or '$' */
                    default:
                    {
                    c_size = tsm_rune_size(&pattern[i]);
                    if (!c_size) return 0;
                    re_compiled[j].type = CHAR;
                    memcpy(re_compiled[j].u.ch, &pattern[i], c_size);
                    re_compiled[j].ch_size = c_size;
                    } break;
                }
            }
            /* '\\' as last char in pattern -> invalid regular expression. */
            /*
            else {
                re_compiled[j].type = CHAR;
                re_compiled[j].ch = pattern[i];
            }
            */
        } break;

        /* Character class: */
        case '[':
        {
            /* Remember where the char-buffer starts. */
            int buf_begin = ccl_bufidx;

            /* Look-ahead to determine if negated */
            if (pattern[i + 1] == '^') {
                re_compiled[j].type = INV_CHAR_CLASS;
                i += 1; /* Increment i to avoid including '^' in the char-buffer */
                if (pattern[i + 1] == 0) /* incomplete pattern, missing non-zero char after '^' */
                    return 0;
            } else {
                re_compiled[j].type = CHAR_CLASS;
            }

            /* Copy characters inside [..] to buffer */
            while (    (pattern[++i] != ']')
                    && (pattern[i]   != '\0')) /* Missing ] */ {
                if (pattern[i] == '\\') {
                    if (ccl_bufidx >= MAX_CHAR_CLASS_LEN - 1) {
                        // fputs("exceeded internal buffer!\n", stderr);
                        return 0;
                    }
                    if (pattern[i + 1] == 0) {
                        /* incomplete pattern, missing non-zero char after '\\' */
                        return 0;
                    }
                    ccl_buf[ccl_bufidx++] = pattern[i++];
                } else if (ccl_bufidx >= MAX_CHAR_CLASS_LEN) {
                    // fputs("exceeded internal buffer!\n", stderr);
                    return 0;
                }
                ccl_buf[ccl_bufidx++] = pattern[i];
            }
            if (ccl_bufidx >= MAX_CHAR_CLASS_LEN) {
                /* Catches cases such as [00000000000000000000000000000000000000][ */
                // fputs("exceeded internal buffer!\n", stderr);
                return 0;
            }
            /* Null-terminate string end */
            ccl_buf[ccl_bufidx++] = 0;
            re_compiled[j].u.ccl = &ccl_buf[buf_begin];
        } break;

        /* Other characters: */
        default:
        {
            re_compiled[j].type = CHAR;
            memcpy(re_compiled[j].u.ch, &pattern[i], c_size);
            re_compiled[j].ch_size = c_size;
        } break;
        }
        /* no buffer-out-of-bounds access on invalid patterns
         * see https://github.com/kokke/tiny-regex-c/commit/1a279e04014b70b0695fba559a7c05d55e6ee90b
         */
        if (pattern[i] == 0)
            return 0;

        i += c_size;
        j += 1;
    }
    /* 'UNUSED' is a sentinel used to indicate end-of-pattern */
    re_compiled[j].type = UNUSED;

    return (re_t) re_compiled;
}

void re_print(regex_t* pattern) {
    const char* types[] = {
        "UNUSED", "DOT", "BEGIN", "END", "QUESTIONMARK", "STAR", "PLUS",
        "CHAR", "CHAR_CLASS", "INV_CHAR_CLASS", "DIGIT", "NOT_DIGIT",
        "ALPHA", "NOT_ALPHA", "WHITESPACE", "NOT_WHITESPACE", "BRANCH",
    };

    int i;
    int j;
    char c;
    for (i = 0; i < MAX_REGEXP_OBJECTS; ++i) {
        if (pattern[i].type == UNUSED)
            break;

        printf("type: %s", types[pattern[i].type]);
        if (pattern[i].type == CHAR_CLASS || pattern[i].type == INV_CHAR_CLASS) {
            printf(" [");
            for (j = 0; j < MAX_CHAR_CLASS_LEN; ++j) {
                c = pattern[i].u.ccl[j];
                if ((c == '\0') || (c == ']'))
                    break;
                printf("%c", c);
            }
            printf("]");
        } else if (pattern[i].type == CHAR) {
            printf(" '%c'", pattern[i].u.ch[0]);
        }
        printf("\n");
    }
}

TsmResult tsm_regex_match(const char *pattern, const char *str) {
    if (pattern == NULL || str == NULL)
        return TSM_FAIL;

    re_t compiled = re_compile(pattern);
    if (!compiled)
        return TSM_SYNTAX_ERROR;

    int matchlength;
    int res = re_matchp(compiled, str, &matchlength);
    return (res == -1 ? TSM_FAIL : TSM_OK);
}


/* Private functions: */
static int matchdigit(char c) {
    return isdigit((unsigned char)c);
}

static int matchalpha(char c) {
    return isalpha((unsigned char)c);
}

static int matchwhitespace(char c) {
    return isspace((unsigned char)c);
}

static int matchalphanum(char c) {
    return ((c == '_') || matchalpha(c) || matchdigit(c));
}

static int matchrange(const char* c, int c_size, const char* str, int rune_size) {
    if ((*c == '-')
        || (str[0] == '\0')
        || (str[0] == '-')
        || (str[rune_size] != '-')
        || (str[rune_size + 1] == '\0'))
        return 0;
    uint32_t c_code = tsm_rune_code(c, c_size);
    uint32_t str_code = tsm_rune_code(str, rune_size);
    const char* str2 = &str[rune_size + 1];
    int rune_size2 = tsm_rune_size(str2);
    if (!rune_size2) return 0;
    uint32_t str_code2 = tsm_rune_code(str2, rune_size2);
    return ((c_code >= str_code) && (c_code <= str_code2));
}

static int matchdot(char c) {
#if defined(RE_DOT_MATCHES_NEWLINE) && (RE_DOT_MATCHES_NEWLINE == 1)
    (void)c;
    return 1;
#else
    return c != '\n' && c != '\r';
#endif
}

static int ismetachar(char c) {
    return ((c == 's') || (c == 'S') || (c == 'w') || (c == 'W') || (c == 'd') || (c == 'D'));
}

static int matchmetachar(const char* c, int c_size, const char* str, int rune_size) {
    switch (str[0]) {
        case 'd': return  matchdigit(*c);
        case 'D': return !matchdigit(*c);
        case 'w': return  matchalphanum(*c);
        case 'W': return !matchalphanum(*c);
        case 's': return  matchwhitespace(*c);
        case 'S': return !matchwhitespace(*c);
        default:  return (c_size == rune_size) && tsm_rune_cmp(c, str, c_size);
    }
}

static int matchcharclass(const char* c, int c_size, const char* str) {
    do {
        int rune_size = tsm_rune_size(str);
        if (!rune_size) return 0;
        if (matchrange(c, c_size, str, rune_size)) {
            return 1;
        } else if (str[0] == '\\') {
            /* Escape-char: increment str-ptr and match on next char */
            str += 1;
            rune_size = tsm_rune_size(str);
            if (!rune_size) return 0;
            if (matchmetachar(c, c_size, str, rune_size))
                return 1;
            else if ((c_size == rune_size) && tsm_rune_cmp(c, str, c_size) && !ismetachar(*c))
                return 1;
        } else if ((c_size == rune_size) && tsm_rune_cmp(c, str, c_size)) {
            if (*c == '-')
                return ((str[-1] == '\0') || (str[1] == '\0'));
            else
                return 1;
        }
        if (*str == '\0')
            break;
        str += rune_size;
    } while (1);

    return 0;
}

static int matchone(regex_t p, const char* c, int c_size) {
    switch (p.type) {
        case DOT:            return matchdot(*c);
        case CHAR_CLASS:     return  matchcharclass(c, c_size, (const char*)p.u.ccl);
        case INV_CHAR_CLASS: return !matchcharclass(c, c_size, (const char*)p.u.ccl);
        case DIGIT:          return  matchdigit(*c);
        case NOT_DIGIT:      return !matchdigit(*c);
        case ALPHA:          return  matchalphanum(*c);
        case NOT_ALPHA:      return !matchalphanum(*c);
        case WHITESPACE:     return  matchwhitespace(*c);
        case NOT_WHITESPACE: return !matchwhitespace(*c);
        case BEGIN:          return 0;
        default:             return  (c_size == p.ch_size) &&
                                     tsm_rune_cmp(c, (const char*)p.u.ch, c_size);
    }
}

static int matchstar(regex_t p, regex_t* pattern,
                     const char* text, int rune_size, int* matchlength) {
    return matchplus(p, pattern, text, rune_size, matchlength) ||
           matchpattern(pattern, text, rune_size, matchlength);
}

static int matchplus(regex_t p, regex_t* pattern,
                     const char* text, int rune_size, int* matchlength) {
    const char* prepoint = text;
    while ((text[0] != '\0') && matchone(p, text, rune_size)) {
        text += rune_size;
        *matchlength += rune_size;
        rune_size = tsm_rune_size(text);
        if (!rune_size) return 0;
    }
    while (text > prepoint) {
        if (matchpattern(pattern, text, rune_size, matchlength)) {
            *matchlength += (int)(text - prepoint);
            return 1;
        }
        do {
            text--;
        } while (text > prepoint && is_multibyte_seq(*text));
        rune_size = tsm_rune_size(text);
        if (!rune_size) return 0;
    }

    return 0;
}

static int matchquestion(regex_t p, regex_t* pattern,
                         const char* text, int rune_size, int* matchlength) {
    if (p.type == UNUSED)
        return 1;
    if (matchpattern(pattern, text, rune_size, matchlength))
        return 1;
    if (!*text)
        return 0;
    int match = matchone(p, text, rune_size);
    text += rune_size;
    if (match) {
        int rune_size2 = tsm_rune_size(text);
        if (!rune_size2) return 0;
        if (matchpattern(pattern, text, rune_size2, matchlength)) {
            *matchlength += rune_size;
            return 1;
        }
    }
    return 0;
}

static int matchpattern(regex_t* pattern, const char* text, int rune_size, int* matchlength) {
    int pre = *matchlength;
    do {
        if ((pattern[0].type == UNUSED) || (pattern[1].type == QUESTIONMARK))
            return matchquestion(pattern[0], &pattern[2], text, rune_size, matchlength);
        else if (pattern[1].type == STAR)
            return matchstar(pattern[0], &pattern[2], text, rune_size, matchlength);
        else if (pattern[1].type == PLUS)
            return matchplus(pattern[0], &pattern[2], text, rune_size, matchlength);
        else if ((pattern[0].type == END) && pattern[1].type == UNUSED)
            return (text[0] == '\0');
    /*  Branching is not working properly
        else if (pattern[1].type == BRANCH)
            return (matchpattern(pattern, text, rune_size) || matchpattern(&pattern[2], text, rune_size));
    */
        *matchlength += rune_size;
        if ((text[0] == '\0') || !matchone(*pattern++, text, rune_size))
            break;
        text += rune_size;
        rune_size = tsm_rune_size(text);
        if (!rune_size) break;
    } while (1);

    *matchlength = pre;
    return 0;
}
