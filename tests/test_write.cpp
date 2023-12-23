#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>
#include <iterator>
#include <string>

TEST(write, minimalWorkbook)
{
    const auto filepath = std::filesystem::path{ "data/out_test_write_minimal.xlsx" };
    std::filesystem::remove(filepath);
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const auto sheet_name = std::string{ "Sheet1" };

    auto workbook = xlsx::workbook{};
    workbook.add_worksheet(sheet_name);

    const auto write_result = xlsx::write(workbook, filepath);
    EXPECT_TRUE(write_result.has_value());
    if (!write_result) {
        FAIL() << write_result.error();
    }
    ASSERT_TRUE(std::filesystem::is_regular_file(filepath));

    const auto maybe_workbook_read = xlsx::read(filepath);
    if (!maybe_workbook_read) {
        FAIL() << maybe_workbook_read.error();
    }
    ASSERT_TRUE(maybe_workbook_read.has_value());
    const auto& workbook_read = maybe_workbook_read.value();
    EXPECT_EQ(workbook_read.application_name(), "xlspp");
    EXPECT_EQ(std::ranges::size(workbook_read), 1);

    const auto* worksheet = workbook_read[sheet_name];
    ASSERT_THAT(worksheet, testing::NotNull());
}
