#include "zip_archive.hpp"

#include <gmock/gmock.h>
#include <range/v3/to_container.hpp>
#include <range/v3/view.hpp>

#include <filesystem>
#include <span>
#include <vector>

namespace xl = xlsx::detail;

namespace {

auto IsFileInfos(std::span<const xl::zip_file_info> expected_file_infos)
    -> testing::Matcher<std::vector<xl::zip_file_info>>
{
    using testing::Field;

    auto to_matcher = [](const auto& expected_file_info) -> testing::Matcher<xl::zip_file_info> {
        return testing::AllOf(
            Field("index", &xl::zip_file_info::index, expected_file_info.index),
            Field("filepath", &xl::zip_file_info::filepath, expected_file_info.filepath),
            Field("encrypted", &xl::zip_file_info::encrypted, expected_file_info.encrypted));
    };

    return testing::ElementsAreArray(
        expected_file_infos | ranges::views::transform(to_matcher) | ranges::to<std::vector>);
}

}   // namespace

TEST(zipArchive, openExistingArchive)
{
    const auto archive_path = std::filesystem::path{ "data/libreoffice_default.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(archive_path));

    const auto maybe_zip_archive = xl::zip_archive::open(archive_path);
    EXPECT_TRUE(maybe_zip_archive.has_value());
}

TEST(zipArchive, tryOpenNonExistingFile)
{
    const auto archive_path = std::filesystem::path{ "data/non-existing-file.txt" };
    std::filesystem::remove(archive_path);
    ASSERT_FALSE(std::filesystem::exists(archive_path));

    const auto maybe_zip_archive = xl::zip_archive::open(archive_path);
    EXPECT_FALSE(maybe_zip_archive.has_value());
}

TEST(zipArchive, tryOpenInvalidFile)
{
    const auto archive_path = std::filesystem::path{ "data/invalid-testfile.txt" };
    ASSERT_TRUE(std::filesystem::exists(archive_path));

    const auto maybe_zip_archive = xl::zip_archive::open(archive_path);
    EXPECT_FALSE(maybe_zip_archive.has_value());
}

TEST(zipArchive, enumerateExistingArchive)
{
    const auto archive_path = std::filesystem::path{ "data/libreoffice_default.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(archive_path));

    const auto maybe_zip_archive = xl::zip_archive::open(archive_path);
    ASSERT_TRUE(maybe_zip_archive.has_value());
    const auto& zip_archive = maybe_zip_archive.value();

    const auto expected_file_infos =
        std::vector<xl::zip_file_info>{ { .index = 0, .filepath = "_rels/.rels" },
                                        { .index = 1, .filepath = "xl/_rels/workbook.xml.rels" },
                                        { .index = 2, .filepath = "xl/workbook.xml" },
                                        { .index = 3, .filepath = "xl/styles.xml" },
                                        { .index = 4, .filepath = "xl/worksheets/sheet1.xml" },
                                        { .index = 5, .filepath = "docProps/core.xml" },
                                        { .index = 6, .filepath = "docProps/app.xml" },
                                        { .index = 7, .filepath = "[Content_Types].xml" } };

    const auto file_infos = zip_archive | ranges::to<std::vector>;
    EXPECT_THAT(file_infos, IsFileInfos(expected_file_infos));
}

TEST(zipArchive, inflate)
{
    const auto archive_path = std::filesystem::path{ "data/libreoffice_default.xlsx" };
    ASSERT_TRUE(std::filesystem::exists(archive_path));

    const auto maybe_zip_archive = xl::zip_archive::open(archive_path);
    ASSERT_TRUE(maybe_zip_archive.has_value());
    const auto& zip_archive = maybe_zip_archive.value();

    const auto archive_file_path = std::string{ "xl/workbook.xml" };
    ASSERT_TRUE(zip_archive.contains(archive_file_path));
    const auto* inflated_file_content = zip_archive.read_all(archive_file_path);
    ASSERT_THAT(inflated_file_content, testing::NotNull());

    const auto expected_content = std::string{
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<workbook "
        "xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
        "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/"
        "relationships\"><fileVersion appName=\"Calc\"/><workbookPr backupFile=\"false\" "
        "showObjects=\"all\" date1904=\"false\"/><workbookProtection/><bookViews><workbookView "
        "showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" "
        "xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" "
        "firstSheet=\"0\" activeTab=\"0\"/></bookViews><sheets><sheet name=\"Sheet1\" "
        "sheetId=\"1\" state=\"visible\" r:id=\"rId2\"/></sheets><calcPr iterateCount=\"100\" "
        "refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/><extLst><ext "
        "xmlns:loext=\"http://schemas.libreoffice.org/\" "
        "uri=\"{7626C862-2A13-11E5-B345-FEFF819CDC9F}\"><loext:extCalcPr "
        "stringRefSyntax=\"CalcA1\"/></ext></extLst></workbook>"
    };
    EXPECT_THAT(*inflated_file_content, testing::StrEq(expected_content));
}

TEST(zipArchive, addFile)
{
    const auto archive_path = std::filesystem::path{ "out.zip" };
    std::filesystem::remove(archive_path);
    ASSERT_FALSE(std::filesystem::exists(archive_path));

    const auto zipped_file_path = std::string{ "test dir/test file.txt" };
    //   const auto file_content = std::string{ "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" };
    const auto file_content = std::string{
        R"(<?xml version="1.0" encoding="UTF-8"?><workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"><fileVersion appName="xlspp "/><sheets><sheet name="Sheet1" sheetId="1" state="visible"/></sheets></workbook>)"
    };
    {
        auto maybe_zip_archive =
            xl::zip_archive::open(archive_path, xl::zip_archive::open_mode::create_if_not_exist);
        EXPECT_FALSE(std::filesystem::exists(archive_path));
        ASSERT_TRUE(maybe_zip_archive.has_value());
        auto& zip_archive = maybe_zip_archive.value();

        zip_archive.add(file_content, zipped_file_path);
    }

    ASSERT_TRUE(std::filesystem::exists(archive_path));

    {
        auto maybe_zip_archive = xl::zip_archive::open(archive_path);
        ASSERT_TRUE(maybe_zip_archive.has_value());
        auto& zip_archive = maybe_zip_archive.value();

        const auto expected_file_infos =
            std::vector{ xl::zip_file_info{ .index = 0, .filepath = zipped_file_path } };

        const auto file_infos = zip_archive | ranges::to<std::vector>;
        EXPECT_THAT(file_infos, IsFileInfos(expected_file_infos));

        ASSERT_TRUE(zip_archive.contains(zipped_file_path));
        const auto* inflated_file_content = zip_archive.read_all(zipped_file_path);
        ASSERT_THAT(inflated_file_content, testing::NotNull());
        EXPECT_EQ(*inflated_file_content, file_content);
    }
}
