#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>

TEST(readInvalid, nonexistingFile)
{
    const std::filesystem::path filepath = "data/non-estisting-file.xlsx";
    ASSERT_FALSE(std::filesystem::exists(filepath));

    const std::optional<xlsx::workbook> maybe_workbook = xlsx::read(filepath);
    EXPECT_FALSE(maybe_workbook.has_value());
}

TEST(readInvalid, invalidFileType)
{
    const std::filesystem::path filepath1 = "data/invalid-testfile.txt";
    ASSERT_TRUE(std::filesystem::exists(filepath1));

    const std::optional<xlsx::workbook> maybe_workbook1 = xlsx::read(filepath1);
    EXPECT_FALSE(maybe_workbook1.has_value());

    const std::filesystem::path filepath2 = "data/invalid-testfile.xlsx";
    ASSERT_TRUE(std::filesystem::exists(filepath2));

    const std::optional<xlsx::workbook> maybe_workbook2 = xlsx::read(filepath2);
    EXPECT_FALSE(maybe_workbook2.has_value());
}

TEST(readInvalid, invalidMissingWorkbook)
{
    const std::filesystem::path filepath = "data/libreoffice_default_missing_workbook.xlsx";
    ASSERT_TRUE(std::filesystem::exists(filepath));

    const std::optional<xlsx::workbook> maybe_workbook = xlsx::read(filepath);
    ASSERT_FALSE(maybe_workbook.has_value());
}
