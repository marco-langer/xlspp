#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>
#include <iterator>
#include <string>

TEST(write, minimalWorkbook)
{
    const auto filepath = std::filesystem::path{ "data/out/test_write_minimal.xlsx" };
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

TEST(write, extendedWorkbook)
{
    const auto filepath = std::filesystem::path{ "data/out/test_write_extended.xlsx" };
    std::filesystem::remove(filepath);
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const auto app_name = "my app";
    const auto sheet_name1 = std::string{ "My Sheet1" };
    const auto sheet_name2 = std::string{ "My Sheet2" };
    const auto sheet_name3 = std::string{ "My Sheet3" };
    auto workbook = xlsx::workbook{};
    workbook.set_application_name(app_name);
    workbook.add_worksheet(sheet_name1);
    auto& sheet2 = workbook.add_worksheet(sheet_name2);
    sheet2.set_visible(false);
    workbook.add_worksheet(sheet_name3);

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
    EXPECT_EQ(workbook_read.application_name(), app_name);
    EXPECT_EQ(std::ranges::size(workbook_read), 3);

    const auto* worksheet1 = workbook_read[sheet_name1];
    ASSERT_THAT(worksheet1, testing::NotNull());
    EXPECT_TRUE(worksheet1->visible());
    const auto* worksheet2 = workbook_read[sheet_name2];
    ASSERT_THAT(worksheet2, testing::NotNull());
    EXPECT_FALSE(worksheet2->visible());
    const auto* worksheet3 = workbook_read[sheet_name3];
    ASSERT_THAT(worksheet3, testing::NotNull());
    EXPECT_TRUE(worksheet3->visible());
}