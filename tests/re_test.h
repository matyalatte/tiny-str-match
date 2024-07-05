#pragma once
#include <stdio.h>
#include <gtest/gtest.h>
#include "str_match.h"


struct RegexCase {
    const char *pattern;
    const char *str;
    int expected;
};

class RegexTest : public ::testing::TestWithParam<RegexCase> {
};

// Test with null pointers.
const RegexCase regex_cases_null[] = {
    { "test", NULL, 0 },
    { NULL, "test", 0 },
    { NULL, NULL, 0 },
};

INSTANTIATE_TEST_CASE_P(RegexTestInstantiation_Null,
    RegexTest,
    ::testing::ValuesIn(regex_cases_null));

// Test cases from tiny-regex-c
// https://github.com/kokke/tiny-regex-c/blob/master/tests/test1.c
const RegexCase regex_cases_ascii[] = {
    { "\\d", "5", 1 },
    { "\\w+", "hej", 1 },
    { "\\s", "\t \n", 1 },
    { "\\S", "\t \n", 0 },
    { "[\\s]", "\t \n", 1 },
    { "[\\S]", "\t \n", 0 },
    { "\\D", "5", 0 },
    { "\\W+", "hej", 0 },
    { "[0-9]+", "12345", 1 },
    { "\\D", "hej", 1 },
    { "\\d", "hej", 0 },
    { "[^\\w]", "\\", 1 },
    { "[\\W]", "\\", 1 },
    { "[\\w]", "\\", 0 },
    { "[^\\d]", "d", 1 },
    { "[\\d]", "d", 0 },
    { "[^\\D]", "d", 0 },
    { "[\\D]", "d", 1 },
    { "^.*\\\\.*$", "c:\\Tools", 1 },
    { "^[\\+-]*[\\d]+$", "+27", 1 },
    { "[abc]", "1c2", 1 },
    { "[abc]", "1C2", 0 },
    { "[1-5]+", "0123456789", 1 },
    { "[.2]", "1C2", 1 },
    { "a*$", "Xaa", 1 },
    { "[a-h]+", "abcdefgjxxx", 1 },
    { "[a-h]+", "ABCDEFGH", 0 },
    { "[A-H]+", "ABCDEFGH", 1 },
    { "[A-H]+", "abcdefgh", 0 },
    { "[^\\s]+", "abc def", 1 },
    { "[^fc]+", "abc def", 1 },
    { "[^d\\sf]+", "abc def", 1 },
    { "\n", "abc\ndef", 1 },
    { "b.\\s*\n", "aa\r\nbb\r\ncc\r\n\r\n", 1 },
    { ".*c", "abcabc", 1 },
    { ".+c", "abcabc", 1 },
    { "[b-z].*", "ab", 1 },
    { "b[k-z]*", "ab", 1 },
    { "[0-9]", "  - ", 0 },
    { "[^0-9]", "  - ", 1 },
    { "0|", "0|", 1 },
    { "\\d\\d:\\d\\d:\\d\\d", "0s:00:00", 0 },
    { "\\d\\d:\\d\\d:\\d\\d", "000:00", 0 },
    { "\\d\\d:\\d\\d:\\d\\d", "00:0000", 0 },
    { "\\d\\d:\\d\\d:\\d\\d", "100:0:00", 0 },
    { "\\d\\d:\\d\\d:\\d\\d", "00:100:00", 0 },
    { "\\d\\d:\\d\\d:\\d\\d", "0:00:100", 0 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:0", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:0", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:00", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:0", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:0", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:00", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:00", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:00", 1 },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "a:0", 0 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "hello world !", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello World !", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world!   ", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world  !", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !", 1 },
    { "[Hh]ello [Ww]orld\\s*[!]?", "hello World    !", 1 },
    { ".?bar", "real_bar", 1 },
    { ".?bar", "real_foo", 0 },
    { "X?Y", "Z", 0 },
    { "[a-z]+\nbreak", "blahblah\nbreak", 1 },
    { "[a-z\\s]+\nbreak", "bla bla \nbreak", 1 },
};

INSTANTIATE_TEST_CASE_P(RegexTestInstantiation_Ascii,
    RegexTest,
    ::testing::ValuesIn(regex_cases_ascii));

// Test with UTF-8 strings.
const RegexCase regex_cases_utf[] = {
    { "^.$", u8"\U0001F600", 1 },  // \U0001F600 == "😀", four-byte
    { u8"^[\U0001F600\u3042]$", u8"\u3042", 1 },  // \u3042 == "あ", three-byte
    { u8"^[\u3042-\u304A]*$", u8"\u3042\u3044\u3046\u3048\u304A", 1 },  // \u304A == "お"
};

INSTANTIATE_TEST_CASE_P(RegexTestInstantiation_UTF,
    RegexTest,
    ::testing::ValuesIn(regex_cases_utf));

TEST_P(RegexTest, tsm_regex_match) {
    const RegexCase test_case = GetParam();
    int actual = tsm_regex_match(test_case.pattern, test_case.str);
    EXPECT_EQ(test_case.expected, actual)
        << "\npattern: " << test_case.pattern << ", str: " << test_case.str << "\n";
}
