#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>

TEST(readInvalid, nonexistingFile)
{
    const auto filepath = std::filesystem::path{ "data/non-estisting-file.xlsx" };
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const auto maybe_workbook = xlsx::read(filepath);
    EXPECT_FALSE(maybe_workbook.has_value());
}

TEST(readInvalid, invalidFileType)
{
    const auto filepath1 = std::filesystem::path{ "data/invalid-testfile.txt" };
    ASSERT_TRUE(std::filesystem::exists(filepath1));

    const auto maybe_workbook1 = xlsx::read(filepath1);
    EXPECT_FALSE(maybe_workbook1.has_value());

    const auto filepath2 = std::filesystem::path{ "data/invalid-testfile.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(filepath2));

    const auto maybe_workbook2 = xlsx::read(filepath2);
    EXPECT_FALSE(maybe_workbook2.has_value());
}

TEST(readInvalid, invalidMissingWorkbook)
{
    const auto filepath = std::filesystem::path{ "data/libreoffice_default_missing_workbook.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(filepath));

    const auto maybe_workbook = xlsx::read(filepath);
    ASSERT_FALSE(maybe_workbook.has_value());
}
