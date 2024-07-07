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

// Test with bad runes.
const RegexCase regex_cases_bad[] = {
    { "a\x81", "a", TSM_SYNTAX_ERROR },
    { "a\xc0 ", "a", TSM_SYNTAX_ERROR },  // two-byte bad rune
    { "a\xe0 ", "a", TSM_SYNTAX_ERROR },  // three-byte bad rune
    { "a\xf0 ", "a", TSM_SYNTAX_ERROR },  // four-byte bad rune
    { "a", "a\x81", TSM_FAIL },
    { "a", "a\xc0 ", TSM_FAIL },  // two-byte bad rune
    { "a", "a\xe0 ", TSM_FAIL },  // three-byte bad rune
    { "a", "a\xf0 ", TSM_FAIL },  // four-byte bad rune
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Bad,
    RegexTest,
    ::testing::ValuesIn(regex_cases_bad));

// Test with invalid patterns.
const RegexCase regex_cases_syntax[] = {
    { "[", "", TSM_SYNTAX_ERROR },
    { "{", "", TSM_SYNTAX_ERROR },
    { "{}", "", TSM_SYNTAX_ERROR },
    { "a{,3}", "", TSM_SYNTAX_ERROR },
    { "a{1,0}", "", TSM_SYNTAX_ERROR },
    { "a{1,a}", "", TSM_SYNTAX_ERROR },
    { u8"a{\U0001F600,a}", "", TSM_SYNTAX_ERROR },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_SyntaxError,
    RegexTest,
    ::testing::ValuesIn(regex_cases_syntax));

// Test cases from tiny-regex-c
// https://github.com/kokke/tiny-regex-c/blob/master/tests/test1.c
const RegexCase regex_cases_trc[] = {
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
    { "\\w{2}", "ab", TSM_OK },
    { "\\w{2,}", "abc", TSM_OK },
    { "\\w{3,}", "ab", TSM_FAIL },
    { "\\w{2,3}", "abc", TSM_OK },
    { "\\w{3,4}", "ab", TSM_FAIL },
    { "0|1", "0", TSM_OK },
    { "0|", "0", TSM_OK },
    { "0|", "", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Trc,
    RegexTest,
    ::testing::ValuesIn(regex_cases_trc));

// Test with UTF-8 strings.
const RegexCase regex_cases_utf[] = {
    { "^.$", u8"\U0001F600", TSM_OK },  // \U0001F600 == "😀", four-byte
    { u8"^[\U0001F600\u3042]$", u8"\u3042", TSM_OK },  // \u3042 == "あ", three-byte
    { u8"^[\u3042-\u304A]*$", u8"\u3042\u3044\u3046\u3048\u304A", TSM_OK },  // \u304A == "お"
    { u8"[\\\U0001F600]+", u8"\U0001F600", TSM_OK },
    { u8"[\\\U0001F600]+", u8"\U0001F600\U0001F600\U0001F600", TSM_OK },
    { u8"[\\\U0001F600]+", "a", TSM_FAIL },
    { u8"\U0001F600{3}", u8"\U0001F600\U0001F600\U0001F600", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_UTF,
    RegexTest,
    ::testing::ValuesIn(regex_cases_utf));

// Test with escaped characters.
const RegexCase regex_cases_escaped[] = {
    { "\t\n\v\r\f\a\\g", "\t\n\v\r\f\ag", TSM_OK },
    { "[\t][\n][\v][\r][\f][\a][\\g]", "\t\n\v\r\f\ag", TSM_OK },
    { "[\t\n\v\r\f\a\\g]*", "\t\n\v\r\f\ag", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Escaped,
    RegexTest,
    ::testing::ValuesIn(regex_cases_escaped));

// Test cases from cpython.
// https://github.com/python/cpython/blob/main/Lib/test/re_tests.py
const RegexCase regex_cases_python[] = {
    { "Python|Perl|Tcl", "Perl", TSM_OK },
    // { ")", "", TSM_SYNTAX_ERROR },  // () operator does not supported yet.
    { "", "", TSM_OK },
    { "abc", "abc", TSM_OK },
    { "abc", "xbc", TSM_FAIL },
    { "abc", "axc", TSM_FAIL },
    { "abc", "abx", TSM_FAIL },
    { "abc", "xabcy", TSM_OK },
    { "abc", "ababc", TSM_OK },
    { "ab*c", "abc", TSM_OK },
    { "ab*bc", "abc", TSM_OK },
    { "ab*bc", "abbc", TSM_OK },
    { "ab*bc", "abbbbc", TSM_OK },
    { "ab+bc", "abbc", TSM_OK },
    { "ab+bc", "abc", TSM_FAIL },
    { "ab+bc", "abq", TSM_FAIL },
    { "ab+bc", "abbbbc", TSM_OK },
    { "ab?bc", "abbc", TSM_OK },
    { "ab?bc", "abc", TSM_OK },
    { "ab?bc", "abbbbc", TSM_FAIL },
    { "ab?c", "abc", TSM_OK },
    { "^abc$", "abc", TSM_OK },
    { "^abc$", "abcc", TSM_FAIL },
    { "^abc", "abcc", TSM_OK },
    { "^abc$", "aabc", TSM_FAIL },
    { "abc$", "aabc", TSM_OK },
    { "^", "abc", TSM_OK },
    { "$", "abc", TSM_OK },
    { "a.c", "abc", TSM_OK },
    { "a.c", "axc", TSM_OK },
    { "a.*c", "axyzc", TSM_OK },
    { "a.*c", "axyzd", TSM_FAIL },
    { "a[bc]d", "abc", TSM_FAIL },
    { "a[bc]d", "abd", TSM_OK },
    { "a[b-d]e", "abd", TSM_FAIL },
    { "a[b-d]e", "ace", TSM_OK },
    { "a[b-d]", "aac", TSM_OK },
    { "a[-d]", "a-", TSM_OK },
    { "a[\\-d]", "a-", TSM_OK },
    // { "a[b-]", "a-", TSM_SYNTAX_ERROR },  // tsm returns TSM_OK for this case.
    { "a[]b", "-", TSM_SYNTAX_ERROR },
    { "a[", "-", TSM_SYNTAX_ERROR },
    { "a\\", "-", TSM_SYNTAX_ERROR },
    // { "abc)", "-", TSM_SYNTAX_ERROR },  // () operator is not supported.
    // { "(abc", "-", TSM_SYNTAX_ERROR },
    { "a]", "a]", TSM_OK },
    // { "a[]]b", "a]b", TSM_OK },  // fail.
    { "a[\\]]b", "a]b", TSM_OK },
    { "a[^bc]d", "aed", TSM_OK },
    { "a[^bc]d", "abd", TSM_FAIL },
    // { "a[^-b]d", "adc", TSM_OK },  // fail.
    { "a[^-b]d", "a-c", TSM_FAIL },
    // { "a[^]b]c", "a]c", TSM_FAIL }, // I don't think this pattern should be supported.
    // { "a[^]b]c", "adc", TSM_OK },

    { "ab|cd", "abc", TSM_OK },
    { "ab|cd", "abcd", TSM_OK },

    { "$b", "b", TSM_FAIL },
    { "a+b+c", "aabbabc", TSM_OK },
    { "[^ab]*", "cde", TSM_OK },
    { "abc", "", TSM_FAIL },
    { "a*", "", TSM_OK },
    { "a|b|c|d|e", "e", TSM_OK },
    { "abcd*efg", "abcdefg", TSM_OK },
    { "ab*", "xabyabbbz", TSM_OK },
    { "ab*", "xayabbbz", TSM_OK },
    { "[abhgefdc]ij", "hij", TSM_OK },
    { "a[bcd]*dcdcde", "adcdcde", TSM_OK },
    { "a[bcd]+dcdcde", "adcdcde", TSM_FAIL },
    { "[a-zA-Z_][z-zA-Z0-9_]*", "alpha", TSM_OK },
    { "multiple words of text", "uh-uh", TSM_FAIL },
    { "multiple words", "multiple words, yeah", TSM_OK },
    { "[k]", "ab", TSM_FAIL },
    { "a[-]?c", "ac", TSM_OK },
    { "\\w+", "--ab_cd0123---", TSM_OK },
    { "[\\w]+", "--ab_cd0123---", TSM_OK },
    { "\\D+", "1234abc5678", TSM_OK },
    { "[\\D+]", "1234abc5678", TSM_OK },
    { "[\\da-fA-F]+", "123abc", TSM_OK },
    // TODO: add more test cases
    // { "", "", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Python,
    RegexTest,
    ::testing::ValuesIn(regex_cases_python));

// Original test cases.
const RegexCase regex_cases_tsm[] = {
    { "^[a-zA-Z_][z-zA-Z0-9_]*", "a", TSM_OK },
    { "^[a-zA-Z_][z-zA-Z0-9_]*", "abcd1234_", TSM_OK },
    { "^[a-zA-Z_][z-zA-Z0-9_]*", "1", TSM_FAIL },
    { "^[a-zA-Z_][z-zA-Z0-9_]*", "1bcd1234_", TSM_FAIL },
    { ".", "", TSM_FAIL },
    { "^.$", "", TSM_FAIL },
    { ".*", "", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Tsm,
    RegexTest,
    ::testing::ValuesIn(regex_cases_tsm));

// Test with {} operators.
const RegexCase regex_cases_times[] = {
    { "ab{0,}bc", "abbbbc", TSM_OK },
    { "ab{1,}bc", "abq", TSM_FAIL },
    { "ab{1,}bc", "abbbbc", TSM_OK },
    { "ab{1,3}bc", "abbbbc", TSM_OK },
    { "ab{3,4}bc", "abbbbc", TSM_OK },
    { "ab{4,5}bc", "abbbbc", TSM_FAIL },
    { "ab{0,1}bc", "abc", TSM_OK },
    { "ab{0,1}c", "abc", TSM_OK },
    { "ab{0,1}c", "ac", TSM_OK },
    { "ab{3}bc", "abbbbc", TSM_OK },
    { "ab{2}bc", "abbbbc", TSM_FAIL },
    { "a{1,}b{1,}c", "aabbabc", TSM_OK },
    { "[A-Z]{0,4}", "", TSM_OK },
    { "[A-Z]{1,4}", "", TSM_FAIL },
    { "[A-Z]{1,4}", "A", TSM_OK },
    { "[A-Z]{1,4}", "ABYZ", TSM_OK },
    { "[A-Z]{1,4}", "ABCDE", TSM_OK },
    { "^[A-Z]{1,4}$", "ABCDE", TSM_FAIL },
    { "^[A-Z]{1,}$", "ABCDE", TSM_OK },
    { "^[A-Z]{5,}$", "ABCD", TSM_FAIL },
    { "[A-Z]{4}", "ABCD", TSM_OK },
    { "[A-Z]{4}", "AbCD", TSM_FAIL },
    { "^[A-Z]{4}$", "ABC", TSM_FAIL },
    { "^[A-Z]{4}$", "ABCDE", TSM_FAIL },
    { "{4}", "", TSM_FAIL },
    { "{4}", "aaaa", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Times,
    RegexTest,
    ::testing::ValuesIn(regex_cases_times));

// Test with | operator.
const RegexCase regex_cases_branch[] = {
    { "cat|dog", "cat", TSM_OK },
    { "cat|dog", "dog", TSM_OK },
    { "cat|dog", "mouse", TSM_FAIL },
    { "^cat|dog$", "catt", TSM_OK },
    { "^cat|dog$", "adog", TSM_OK },
    { "^cat|dog$", "acat", TSM_FAIL },
    { "^cat|dog$", "dogg", TSM_FAIL },
    { "[a-z]|D", "b", TSM_OK },
    { "[a-z]|D", "D", TSM_OK },
    { "[a-z]|D", "1", TSM_FAIL },
    { "\\d|\\s", " ", TSM_OK },
    { "\\d|\\s", "0", TSM_OK },
    { "\\d|\\s", "a", TSM_FAIL },
    { "ab*|c", "a", TSM_OK },
    { "ab+|c", "a", TSM_FAIL },
    { "ab*|c", "ab", TSM_OK },
    { "ab*|c", "abbbb", TSM_OK },
    { "ab*|c", "bbbb", TSM_FAIL },
    { "ab*|c", "c", TSM_OK },
    { "^c$|^go$|^rust$", "c", TSM_OK },
    { "^c$|^go$|^rust$", "ac", TSM_FAIL },
    { "^c$|^go$|^rust$", "cpp", TSM_FAIL },
    { "^c$|^go$|^rust$", "go", TSM_OK },
    { "^c$|^go$|^rust$", "togo", TSM_FAIL },
    { "^c$|^go$|^rust$", "goto", TSM_FAIL },
    { "^c$|^go$|^rust$", "rust", TSM_OK },
    { "^c$|^go$|^rust$", "crust", TSM_FAIL },
    { "^c$|^go$|^rust$", "rustc", TSM_FAIL },
    { "[a-z]*\\.png|[a-z]*\\.jpg|[a-z]*\\.bmp", "alpha.jpg", TSM_OK },
    { "^[\\d]{1,4}$|a+", "0123", TSM_OK },
    { "^[\\d]{1,4}$|a+", "01234", TSM_FAIL },
    { "^[\\d]{1,4}$|a+", "aaaaa", TSM_OK },
    { "^[\\d]{1,4}$|a+", "AAAAA", TSM_FAIL },
    { "^[\\d]{1,4}$|a+", "", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Branch,
    RegexTest,
    ::testing::ValuesIn(regex_cases_branch));

// Test with ^$ operators.
const RegexCase regex_cases_startend[] = {
    { "^", "", TSM_OK },
    { "^", "a", TSM_OK },
    { "^a", "a", TSM_OK },
    { "^a", "b", TSM_FAIL },
    { "a^", "", TSM_FAIL },
    { "a^", "a", TSM_FAIL },
    { "a^", "aa", TSM_FAIL },
    { "$", "", TSM_OK },
    { "$", "a", TSM_OK },
    { "a$", "a", TSM_OK },
    { "a$", "b", TSM_FAIL },
    { "$a", "", TSM_FAIL },
    { "$a", "a", TSM_FAIL },
    { "$a", "aa", TSM_FAIL },
    { "$^", "", TSM_FAIL },
    { "^$", "", TSM_OK },
    { "^ab$", "", TSM_FAIL },
    { "^ab$", "ab", TSM_OK },
    { "^ab$", "abb", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(RegexTestInstantiation_Startend,
    RegexTest,
    ::testing::ValuesIn(regex_cases_startend));


TEST_P(RegexTest, tsm_regex_match) {
    const RegexCase test_case = GetParam();
    int actual = tsm_regex_match(test_case.pattern, test_case.str);
    EXPECT_EQ(test_case.expected, actual)
        << "\npattern: " << test_case.pattern << ", str: " << test_case.str << "\n";
}
