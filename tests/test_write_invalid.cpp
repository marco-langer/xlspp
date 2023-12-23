#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>

TEST(writeInvalid, emptyPath)
{
    auto workbook = xlsx::workbook{};
    workbook.add_worksheet("Sheet1");

    const auto filepath = std::filesystem::path{};
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const auto result = xlsx::write(workbook, filepath);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "filepath empty");
    EXPECT_FALSE(std::filesystem::exists(filepath));
}

TEST(writeInvalid, emptyWorkbook)
{
    const auto workbook = xlsx::workbook{};
    const auto filepath = std::filesystem::path{ "emptyWorkbook.xlsx" };
    std::filesystem::remove(filepath);
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const auto result = xlsx::write(workbook, filepath);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "workbook empty");
    EXPECT_FALSE(std::filesystem::exists(filepath));
}
