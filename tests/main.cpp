#include <stdio.h>
#include <gtest/gtest.h>
#include "str_match.h"
#include "wildcard_test.h"
#include "re_test.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
