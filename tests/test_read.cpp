#include "xlspp/xlspp.hpp"

#include <gmock/gmock.h>

#include <filesystem>

namespace {

auto worksheet_names(const xlsx::workbook& workbook) -> std::vector<std::string>
{
    auto names = std::vector<std::string>{};
    for (const auto& worksheet : workbook) {
        names.push_back(worksheet.name());
    }
    return names;
}

}   // namespace

TEST(read, defaultFileLibreOffice)
{
    const auto filepath = std::filesystem::path{ "data/libreoffice_default.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(filepath));

    const auto maybe_workbook = xlsx::read(filepath);
    ASSERT_TRUE(maybe_workbook.has_value());

    const auto& workbook = maybe_workbook.value();
    EXPECT_EQ(workbook.application_name(), "Calc");

    const auto expected_worksheet_name = std::string{ "Sheet1" };
    EXPECT_THAT(worksheet_names(workbook), testing::ElementsAre(expected_worksheet_name));

    const auto* worksheet = workbook[expected_worksheet_name];
    ASSERT_THAT(worksheet, testing::NotNull());
    EXPECT_EQ(worksheet->name(), expected_worksheet_name);
    EXPECT_TRUE(worksheet->visible());
    EXPECT_NO_THROW(auto& expected_worksheet = workbook.get_worksheet(expected_worksheet_name));
}

TEST(read, defaultFilePythonXlsxWriter)
{
    const auto filepath = std::filesystem::path{ "data/python_xlsxwriter_default.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(filepath));

    const auto maybe_workbook = xlsx::read(filepath);
    ASSERT_TRUE(maybe_workbook.has_value());

    const auto& workbook = maybe_workbook.value();
    EXPECT_EQ(workbook.application_name(), "xl");

    const auto expected_worksheet_name = std::string{ "Sheet1" };
    EXPECT_THAT(worksheet_names(workbook), testing::ElementsAre(expected_worksheet_name));

    const auto* worksheet = workbook[expected_worksheet_name];
    ASSERT_THAT(worksheet, testing::NotNull());
    EXPECT_EQ(worksheet->name(), expected_worksheet_name);
    EXPECT_TRUE(worksheet->visible());
    EXPECT_NO_THROW(auto& expected_worksheet = workbook.get_worksheet(expected_worksheet_name));
}

TEST(read, exampleFile)
{
    const auto maybe_workbook = xlsx::read("data/Untitled 1.xlsx");
    if (!maybe_workbook) {
        FAIL() << maybe_workbook.error();
    }
    ASSERT_TRUE(maybe_workbook.has_value());
    const auto& workbook = maybe_workbook.value();
    EXPECT_THAT(worksheet_names(workbook), testing::ElementsAre("My First Sheet", "Sheet2"));

    const xlsx::worksheet* worksheet1 = workbook["My First Sheet"];
    ASSERT_THAT(worksheet1, testing::NotNull());
    EXPECT_EQ(worksheet1->name(), "My First Sheet");
    EXPECT_TRUE(worksheet1->visible());
    EXPECT_EQ(worksheet1->cell_count(), 8);
    EXPECT_NO_THROW(worksheet1->at("A1"));
    EXPECT_NO_THROW(worksheet1->at("A2"));
    EXPECT_NO_THROW(worksheet1->at("A3"));
    EXPECT_NO_THROW(worksheet1->at("A4"));
    EXPECT_NO_THROW(worksheet1->at("B1"));
    EXPECT_TRUE(worksheet1->at("B2").is_number());
    ASSERT_THAT(worksheet1->at("B2").if_number(), testing::NotNull());
    EXPECT_EQ(*worksheet1->at("B2").if_number(), 0.1);
    EXPECT_TRUE(worksheet1->at("B3").is_number());
    ASSERT_THAT(worksheet1->at("B3").if_number(), testing::NotNull());
    EXPECT_EQ(*worksheet1->at("B3").if_number(), 0.2);
    EXPECT_TRUE(worksheet1->at("B4").is_number());
    ASSERT_THAT(worksheet1->at("B4").if_number(), testing::NotNull());
    EXPECT_EQ(*worksheet1->at("B4").if_number(), 4);

    const xlsx::worksheet* worksheet2 = workbook["Sheet2"];
    ASSERT_THAT(worksheet2, testing::NotNull());
    EXPECT_EQ(worksheet2->name(), "Sheet2");
    EXPECT_TRUE(worksheet2->visible());
    EXPECT_EQ(worksheet2->cell_count(), 0);
}
