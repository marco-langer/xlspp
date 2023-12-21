#include "worksheet_utils.hpp"

#include <gmock/gmock.h>

struct WorkSheetName : testing::TestWithParam<std::string>
{};

TEST_P(WorkSheetName, isSheetNameValid)
{
    const auto& sheet_name = GetParam();
    EXPECT_FALSE(xlsx::detail::is_sheet_name_valid(sheet_name));
}

INSTANTIATE_TEST_SUITE_P(
    WorkSheetUtils,
    WorkSheetName,
    testing::Values(
        "",
        "[",
        "]",
        "*",
        "?",
        ":",
        "/",
        "\\",
        "'",
        "[a",
        "]a",
        "*a",
        "?a",
        ":a",
        "/a",
        "\\a",
        "'a",
        "a[",
        "a]",
        "a*",
        "a?",
        "a:",
        "a/",
        "a\\",
        "a'",
        "history",
        "History",
        "hisTory"));
