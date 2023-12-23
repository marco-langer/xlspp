#include "workbook_utils.cpp"

#include <gmock/gmock.h>
#include <tinyxml2.h>

#include <string>

TEST(workbookUtils, serializeWorkbook)
{
    auto workbook = xlsx::workbook{};
    workbook.add_worksheet("Sheet1");

    auto xml_document = tinyxml2::XMLDocument{};
    const auto buffer = xlsx::detail::serialize_workbook(workbook, xml_document);

    const auto expected_buffer = std::string{
        R"(<?xml version="1.0" encoding="UTF-8"?><workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"><fileVersion appName="xlspp"/><sheets><sheet name="Sheet1" sheetId="1" state="visible"/></sheets></workbook>)"
    };
    EXPECT_EQ(buffer, expected_buffer);
}