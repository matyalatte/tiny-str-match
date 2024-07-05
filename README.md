# tiny-str-match

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

tiny-str-match is a small and portable C library for string validation with regex and wildcard.

## Example

```c
#include <stdio.h>
#include "str_match.h"

int main(void) {
    int res;

    // regex
    res = tsm_regex_match("^[A-Z]*$", "ABCDE");
    if (res == TSM_OK)
        printf("valid\n");
    else if (res == TSM_FAIL)
        printf("invalid\n");
    else if (res == TSM_SYNTAX_ERROR)
        printf("failed to compile a regex pattern.\n");

    // wildcard
    res = tsm_wildcard_match("hello?world*", "hello world!");
    if (res == TSM_OK)
        printf("valid\n");
    else
        printf("invalid\n");

    // You can use UTF-8 characters.
    // tsm_regex_match("^[あ-お]*$", "あいうえお") == TSM_OK
    res = tsm_regex_match(u8"^[\u3042-\u304A]*$",
                          u8"\u3042\u3044\u3046\u3048\u304A");
    // tsm_wildcard_match("😀?😂", "😀😁😂") == TSM_OK
    res = tsm_wildcard_match(u8"\U0001F600?\U0001F602",
                             u8"\U0001F600\U0001F601\U0001F602")
    return 0;
}
```

## Supported regex-operators

-   `.`         Dot, matches any character (including multi-byte characters)
-   `^`         Start anchor, matches beginning of string
-   `$`         End anchor, matches end of string
-   `*`         Asterisk, match zero or more (greedy)
-   `+`         Plus, match one or more (greedy)
-   `?`         Question, match zero or one (non-greedy)
-   `{n}`       Exact Quantifier
-   `{n,}`      Match n or more times
-   `{n,m}`     Match n to m times
-   `[abc]`     Character class, match if one of {'a', 'b', 'c'}
-   `[^abc]`   Inverted class, match if NOT one of {'a', 'b', 'c'}
-   `[a-zA-Z]` Character ranges, the character set of the ranges { a-z | A-Z }
-   `\s`       Whitespace, \t \f \r \n \v and spaces
-   `\S`       Non-whitespace
-   `\w`       Alphanumeric, [a-zA-Z0-9_]
-   `\W`       Non-alphanumeric
-   `\d`       Digits, [0-9]
-   `\D`       Non-digits
-   `|`        Branch Or, e.g. a|A, \w|\s

## Supported wildcard-operators

-   `?`         Question, matches any character (including multi-byte characters)
-   `*`         Asterisk, match zero or more

## Building

### Requirements

- [Meson](https://mesonbuild.com/) for building

### Build whole project

```bash
meson setup build
meson compile -C build
meson test -C build
```

### Build library only

```bash
meson setup build -Dtests=false
meson compile -C build
```

### Build as subproject

You don't need to clone the git repo if you build your project with meson.  
Save the following text as `subprojects/tiny_str_match.wrap`.  

```ini
[wrap-git]
url = https://github.com/matyalatte/tiny-str-match.git
revision = head
depth = 1

[provide]
tiny_str_match = tiny_str_match_dep
```

Then, you can use tiny-str-match in your meson project.

```python
tiny_str_match_dep = dependency('tiny_str_match', fallback : ['tiny_str_match', 'tiny_str_match_dep'])
executable('your_exe_name', ['your_code.cpp'], dependencies : [tiny_str_match_dep])
```

```bash
meson setup build -Dtiny_str_match:tests=false
meson compile -C build
```

## Credits

- [kokke/tiny-regex-c](https://github.com/kokke/tiny-regex-c) for the basic implementation of regex module.
- [rurban/tiny-regex-c](https://github.com/rurban/tiny-regex-c) for the basic implementation of some regex operators.
- [tinymatch.c](https://github.com/r-lyeh/tinybits/blob/master/tinymatch.c) by [r-lyeh](https://github.com/r-lyeh) for the basic implementation of wildcard module.

## Projects which use tiny-str-match

- [Tuw](https://github.com/matyalatte/tuw): A tiny GUI wrapper for command-line tools.
