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
    { "test", NULL, TSM_FAIL },
    { NULL, "test", TSM_FAIL },
    { NULL, NULL, TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Null,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_null));

// Test with bad runes.
const WildcardCase wildcard_cases_bad[] = {
    { "a", "a\x81", TSM_FAIL },
    { "a\x81", "a\x81", TSM_FAIL },
    { "a*", "a\x81", TSM_FAIL },
    { "a\xc0 ", "a\xc0 ", TSM_FAIL },  // two-byte bad rune
    { "a\xe0 ", "a\xe0 ", TSM_FAIL },  // three-byte bad rune
    { "a\xf0 ", "a\xf0 ", TSM_FAIL },  // four-byte bad rune
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Bad,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_bad));

// Test with patterns have no wildcard characters.
const WildcardCase wildcard_cases_nocard[] = {
    { "", "", TSM_OK },
    { "", "test", TSM_FAIL },
    { "test", "", TSM_FAIL },
    { "test", "test", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Nocard,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_nocard));

// Test with patterns start with a wildcard character.
const WildcardCase wildcard_cases_first[] = {
    { "*case", "", TSM_FAIL },
    { "*case", "test", TSM_FAIL },
    { "*case", "case", TSM_OK },
    { "*case", "fcase", TSM_OK },
    { "*case", "foocase", TSM_OK },
    { "?case", "", TSM_FAIL },
    { "?case", "test", TSM_FAIL },
    { "?case", "case", TSM_FAIL },
    { "?case", "fcase", TSM_OK },
    { "?case", "foocase", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_First,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_first));

// Test with patterns have a wildcard character.
const WildcardCase wildcard_cases_mid[] = {
    { "test*case", "", TSM_FAIL },
    { "test*case", "test", TSM_FAIL },
    { "test*case", "case", TSM_FAIL },
    { "test*case", "testcase", TSM_OK },
    { "test*case", "testfcase", TSM_OK },
    { "test*case", "testfoocase", TSM_OK },
    { "test?case", "", TSM_FAIL },
    { "test?case", "test", TSM_FAIL },
    { "test?case", "case", TSM_FAIL },
    { "test?case", "testcase", TSM_FAIL },
    { "test?case", "testfcase", TSM_OK },
    { "test?case", "testfoocase", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Mid,
                        WildcardTest,
                        ::testing::ValuesIn(wildcard_cases_mid));

// Test with patterns end with a wildcard character.
const WildcardCase wildcard_cases_last[] = {
    { "test*", "", TSM_FAIL },
    { "test*", "test", TSM_OK },
    { "test*", "case", TSM_FAIL },
    { "test*", "testf", TSM_OK },
    { "test*", "testfoo", TSM_OK },
    { "test?", "", TSM_FAIL },
    { "test?", "test", TSM_FAIL },
    { "test?", "case", TSM_FAIL },
    { "test?", "testf", TSM_OK },
    { "test?", "testfoo", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Last,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_last));

// Test with UTF8 strings.
const WildcardCase wildcard_cases_utf[] = {
    { "test*case", u8"test\u00c4case", TSM_OK },  // \u00c4 == "Ä", two-byte
    { "test*case", u8"test\u00c4\u00c4case", TSM_OK },
    { "test?case", u8"test\u00c4case", TSM_OK },
    { "test?case", u8"test\u00c4\u00c4case", TSM_FAIL },
    { u8"\u00c4*\u00c4", u8"\u00c4\u00c4", TSM_OK },
    { u8"\u00c4*\u00c4", u8"\u00c4a\u00c4", TSM_OK },
    { u8"\u00c4?\u00c4", u8"\u00c4\u00c4", TSM_FAIL },
    { u8"\u00c4?\u00c4", u8"\u00c4a\u00c4", TSM_OK },
    { "test*case", u8"test\u3042case", TSM_OK },  // \u3042 == "あ", three-byte
    { "test*case", u8"test\u3042\u3042case", TSM_OK },
    { "test?case", u8"test\u3042case", TSM_OK },
    { "test?case", u8"test\u3042\u3042case", TSM_FAIL },
    { u8"\u3042*\u3042", u8"\u3042\u3042", TSM_OK },
    { u8"\u3042*\u3042", u8"\u3042a\u3042", TSM_OK },
    { u8"\u3042?\u3042", u8"\u3042\u3042", TSM_FAIL },
    { u8"\u3042?\u3042", u8"\u3042a\u3042", TSM_OK },
    { "test*case", u8"test\U0001F600case", TSM_OK },  // \U0001F600 == "😀", four-byte
    { "test*case", u8"test\U0001F600\U0001F600case", TSM_OK },
    { "test?case", u8"test\U0001F600case", TSM_OK },
    { "test?case", u8"test\U0001F600\U0001F600case", TSM_FAIL },
    { u8"\U0001F600*\U0001F600", u8"\U0001F600\U0001F600", TSM_OK },
    { u8"\U0001F600*\U0001F600", u8"\U0001F600a\U0001F600", TSM_OK },
    { u8"\U0001F600?\U0001F600", u8"\U0001F600\U0001F600", TSM_FAIL },
    { u8"\U0001F600?\U0001F600", u8"\U0001F600a\U0001F600", TSM_OK },
    { u8"\u00c4*\U0001F600", u8"\u00c4\U0001F600", TSM_OK },
    { u8"\u00c4*\U0001F600", u8"\u00c4\u3042\U0001F600", TSM_OK },
    { u8"\u00c4?\U0001F600", u8"\u00c4\U0001F600", TSM_FAIL },
    { u8"\u00c4?\U0001F600", u8"\u00c4\u3042\U0001F600", TSM_OK },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_UTF,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_utf));

// Test with patterns have multiple wildcard characters.
const WildcardCase wildcard_cases_multicard[] = {
    { "test*case*foo?bar??", "testcasefooabarbc", TSM_OK },
    { "test*case*foo?bar??", "testfoocasebarfooabarbc", TSM_OK },
    { "test*case*foo?bar??", "testcasefoobarbc", TSM_FAIL },
    { "test*case*foo?bar??", "testcasefooabarb", TSM_FAIL },
    { "test*case*foo?bar??", "testcasfooabarbc", TSM_FAIL },
    { "test*case*foo?bar??", "tescasefooabarbc", TSM_FAIL },
};

INSTANTIATE_TEST_SUITE_P(WildcardTestInstantiation_Multicard,
    WildcardTest,
    ::testing::ValuesIn(wildcard_cases_multicard));

TEST_P(WildcardTest, tsm_wildcard_match) {
    const WildcardCase test_case = GetParam();
    int actual = tsm_wildcard_match(test_case.pattern, test_case.str);
    EXPECT_EQ(test_case.expected, actual)
        << "\npattern: " << test_case.pattern << ", str: " << test_case.str << "\n";
}
