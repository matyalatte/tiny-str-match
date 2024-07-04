#pragma once
#include <stdio.h>
#include <gtest/gtest.h>
#include "str_match.h"

struct WildcardCase {
    const char *pattern;
    const char *str;
    int expected;
};

class WildcardTest : public ::testing::TestWithParam<WildcardCase> {
};

// Test with null pointers.
const WildcardCase wildcard_cases_null[] = {
    { "test", NULL, 0 },
    { NULL, "test", 0 },
    { NULL, NULL, 0 },
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Null,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_null));

// Test with bad runes.
const WildcardCase wildcard_cases_bad[] = {
    { "a", "a\x81", 0},
    { "a\x81", "a\x81", 0},
    { "a*", "a\x81", 0},
    { "a\xc0 ", "a\xc0 ", 0},  // two-byte bad rune
    { "a\xe0 ", "a\xe0 ", 0},  // three-byte bad rune
    { "a\xf0 ", "a\xf0 ", 0},  // four-byte bad rune
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Bad,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_bad));

// Test with patterns have no wildcard characters.
const WildcardCase wildcard_cases_nocard[] = {
    { "", "", 1},
    { "", "test", 0},
    { "test", "", 0},
    { "test", "test", 1},
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Nocard,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_nocard));

// Test with patterns start with a wildcard character.
const WildcardCase wildcard_cases_first[] = {
    { "*case", "", 0 },
    { "*case", "test", 0 },
    { "*case", "case", 1 },
    { "*case", "fcase", 1 },
    { "*case", "foocase", 1 },
    { "?case", "", 0 },
    { "?case", "test", 0 },
    { "?case", "case", 0 },
    { "?case", "fcase", 1 },
    { "?case", "foocase", 0 },
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_First,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_first));

// Test with patterns have a wildcard character.
const WildcardCase wildcard_cases_mid[] = {
    { "test*case", "", 0 },
    { "test*case", "test", 0 },
    { "test*case", "case", 0 },
    { "test*case", "testcase", 1 },
    { "test*case", "testfcase", 1 },
    { "test*case", "testfoocase", 1 },
    { "test?case", "", 0 },
    { "test?case", "test", 0 },
    { "test?case", "case", 0 },
    { "test?case", "testcase", 0 },
    { "test?case", "testfcase", 1 },
    { "test?case", "testfoocase", 0 },
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Mid,
                        WildcardTest,
                        ::testing::ValuesIn(wildcard_cases_mid));

// Test with patterns end with a wildcard character.
const WildcardCase wildcard_cases_last[] = {
    { "test*", "", 0 },
    { "test*", "test", 1 },
    { "test*", "case", 0 },
    { "test*", "testf", 1 },
    { "test*", "testfoo", 1 },
    { "test?", "", 0 },
    { "test?", "test", 0 },
    { "test?", "case", 0 },
    { "test?", "testf", 1 },
    { "test?", "testfoo", 0 },
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Last,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_last));

// Test with UTF8 strings.
const WildcardCase wildcard_cases_utf[] = {
    { "test*case", u8"test\u00c4case", 1 },  // \u00c4 == "Ä", two-byte
    { "test*case", u8"test\u00c4\u00c4case", 1 },
    { "test?case", u8"test\u00c4case", 1 },
    { "test?case", u8"test\u00c4\u00c4case", 0 },
    { u8"\u00c4*\u00c4", u8"\u00c4\u00c4", 1 },
    { u8"\u00c4*\u00c4", u8"\u00c4a\u00c4", 1 },
    { u8"\u00c4?\u00c4", u8"\u00c4\u00c4", 0 },
    { u8"\u00c4?\u00c4", u8"\u00c4a\u00c4", 1 },
    { "test*case", u8"test\u3042case", 1 },  // \u3042 == "あ", three-byte
    { "test*case", u8"test\u3042\u3042case", 1 },
    { "test?case", u8"test\u3042case", 1 },
    { "test?case", u8"test\u3042\u3042case", 0 },
    { u8"\u3042*\u3042", u8"\u3042\u3042", 1 },
    { u8"\u3042*\u3042", u8"\u3042a\u3042", 1 },
    { u8"\u3042?\u3042", u8"\u3042\u3042", 0 },
    { u8"\u3042?\u3042", u8"\u3042a\u3042", 1 },
    { "test*case", u8"test\U0001F600case", 1 },  // \U0001F600 == "😀", four-byte
    { "test*case", u8"test\U0001F600\U0001F600case", 1 },
    { "test?case", u8"test\U0001F600case", 1 },
    { "test?case", u8"test\U0001F600\U0001F600case", 0 },
    { u8"\U0001F600*\U0001F600", u8"\U0001F600\U0001F600", 1 },
    { u8"\U0001F600*\U0001F600", u8"\U0001F600a\U0001F600", 1 },
    { u8"\U0001F600?\U0001F600", u8"\U0001F600\U0001F600", 0 },
    { u8"\U0001F600?\U0001F600", u8"\U0001F600a\U0001F600", 1 },
    { u8"\u00c4*\U0001F600", u8"\u00c4\U0001F600", 1 },
    { u8"\u00c4*\U0001F600", u8"\u00c4\u3042\U0001F600", 1 },
    { u8"\u00c4?\U0001F600", u8"\u00c4\U0001F600", 0 },
    { u8"\u00c4?\U0001F600", u8"\u00c4\u3042\U0001F600", 1 },
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_UTF,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_utf));

// Test with patterns have multiple wildcard characters.
const WildcardCase wildcard_cases_multicard[] = {
    { "test*case*foo?bar??", "testcasefooabarbc", 1},
    { "test*case*foo?bar??", "testfoocasebarfooabarbc", 1},
    { "test*case*foo?bar??", "testcasefoobarbc", 0},
    { "test*case*foo?bar??", "testcasefooabarb", 0},
    { "test*case*foo?bar??", "testcasfooabarbc", 0},
    { "test*case*foo?bar??", "tescasefooabarbc", 0},
};

INSTANTIATE_TEST_CASE_P(WildcardTestInstantiation_Multicard,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_multicard));

TEST_P(WildcardTest, tsmWildcardMatch) {
    const WildcardCase test_case = GetParam();
    int actual = tsmWildcardMatch(test_case.pattern, test_case.str);
    EXPECT_EQ(test_case.expected, actual)
        << "\npattern: " << test_case.pattern << ", str: " << test_case.str << "\n";
}
