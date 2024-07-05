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
    { "test", NULL, TSM_FAIL },
    { NULL, "test", TSM_FAIL },
    { NULL, NULL, TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Null,
    RegexTest,
    ::testing::ValuesIn(regex_cases_null));

// Test cases from tiny-regex-c
// https://github.com/kokke/tiny-regex-c/blob/master/tests/test1.c
const RegexCase regex_cases_ascii[] = {
    { "\\d", "5", TSM_OK },
    { "\\w+", "hej", TSM_OK },
    { "\\s", "\t \n", TSM_OK },
    { "\\S", "\t \n", TSM_FAIL },
    { "[\\s]", "\t \n", TSM_OK },
    { "[\\S]", "\t \n", TSM_FAIL },
    { "\\D", "5", TSM_FAIL },
    { "\\W+", "hej", TSM_FAIL },
    { "[0-9]+", "12345", TSM_OK },
    { "\\D", "hej", TSM_OK },
    { "\\d", "hej", TSM_FAIL },
    { "[^\\w]", "\\", TSM_OK },
    { "[\\W]", "\\", TSM_OK },
    { "[\\w]", "\\", TSM_FAIL },
    { "[^\\d]", "d", TSM_OK },
    { "[\\d]", "d", TSM_FAIL },
    { "[^\\D]", "d", TSM_FAIL },
    { "[\\D]", "d", TSM_OK },
    { "^.*\\\\.*$", "c:\\Tools", TSM_OK },
    { "^[\\+-]*[\\d]+$", "+27", TSM_OK },
    { "[abc]", "1c2", TSM_OK },
    { "[abc]", "1C2", TSM_FAIL },
    { "[1-5]+", "0123456789", TSM_OK },
    { "[.2]", "1C2", TSM_OK },
    { "a*$", "Xaa", TSM_OK },
    { "[a-h]+", "abcdefgjxxx", TSM_OK },
    { "[a-h]+", "ABCDEFGH", TSM_FAIL },
    { "[A-H]+", "ABCDEFGH", TSM_OK },
    { "[A-H]+", "abcdefgh", TSM_FAIL },
    { "[^\\s]+", "abc def", TSM_OK },
    { "[^fc]+", "abc def", TSM_OK },
    { "[^d\\sf]+", "abc def", TSM_OK },
    { "\n", "abc\ndef", TSM_OK },
    { "b.\\s*\n", "aa\r\nbb\r\ncc\r\n\r\n", TSM_OK },
    { ".*c", "abcabc", TSM_OK },
    { ".+c", "abcabc", TSM_OK },
    { "[b-z].*", "ab", TSM_OK },
    { "b[k-z]*", "ab", TSM_OK },
    { "[0-9]", "  - ", TSM_FAIL },
    { "[^0-9]", "  - ", TSM_OK },
    { "0|", "0|", TSM_OK },
    { "\\d\\d:\\d\\d:\\d\\d", "0s:00:00", TSM_FAIL },
    { "\\d\\d:\\d\\d:\\d\\d", "000:00", TSM_FAIL },
    { "\\d\\d:\\d\\d:\\d\\d", "00:0000", TSM_FAIL },
    { "\\d\\d:\\d\\d:\\d\\d", "100:0:00", TSM_FAIL },
    { "\\d\\d:\\d\\d:\\d\\d", "00:100:00", TSM_FAIL },
    { "\\d\\d:\\d\\d:\\d\\d", "0:00:100", TSM_FAIL },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:0", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:0", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:00", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:0", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:0", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:00", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:00", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:00", TSM_OK },
    { "\\d\\d?:\\d\\d?:\\d\\d?", "a:0", TSM_FAIL },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "hello world !", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello World !", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world!   ", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world  !", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !", TSM_OK },
    { "[Hh]ello [Ww]orld\\s*[!]?", "hello World    !", TSM_OK },
    { ".?bar", "real_bar", TSM_OK },
    { ".?bar", "real_foo", TSM_FAIL },
    { "X?Y", "Z", TSM_FAIL },
    { "[a-z]+\nbreak", "blahblah\nbreak", TSM_OK },
    { "[a-z\\s]+\nbreak", "bla bla \nbreak", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Ascii,
    RegexTest,
    ::testing::ValuesIn(regex_cases_ascii));

// Test with UTF-8 strings.
const RegexCase regex_cases_utf[] = {
    { "^.$", u8"\U0001F600", TSM_OK },  // \U0001F600 == "😀", four-byte
    { u8"^[\U0001F600\u3042]$", u8"\u3042", TSM_OK },  // \u3042 == "あ", three-byte
    { u8"^[\u3042-\u304A]*$", u8"\u3042\u3044\u3046\u3048\u304A", TSM_OK },  // \u304A == "お"
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_UTF,
    RegexTest,
    ::testing::ValuesIn(regex_cases_utf));

TEST_P(RegexTest, tsm_regex_match) {
    const RegexCase test_case = GetParam();
    int actual = tsm_regex_match(test_case.pattern, test_case.str);
    EXPECT_EQ(test_case.expected, actual)
        << "\npattern: " << test_case.pattern << ", str: " << test_case.str << "\n";
}
