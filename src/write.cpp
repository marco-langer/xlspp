#include "xlspp/write.hpp"

#include "zip_archive.hpp"

#include <fmt/core.h>
#include <tinyxml2.h>

#include <algorithm>
#include <fstream>    // TODO remove
#include <iostream>   // TODO remove
#include <string>

namespace {

auto serialize_workbook(
    std::string& buffer, tinyxml2::XMLDocument& xml_document, const xlsx::workbook& workbook)
{
    buffer.clear();
    xml_document.Clear();

    // Note: this xml declaration is missing the "standalone" attribute, which however
    // seems to be no problem
    xml_document.InsertFirstChild(xml_document.NewDeclaration());

    auto* workbook_element = xml_document.NewElement("workbook");
    workbook_element->SetAttribute(
        "xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    workbook_element->SetAttribute(
        "xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");

    auto* file_version_element = workbook_element->InsertNewChildElement("fileVersion");
    file_version_element->SetAttribute("appName", workbook.application_name().c_str());

    auto* sheets_element = workbook_element->InsertNewChildElement("sheets");
    for (const auto& worksheet : workbook) {
        auto* sheet_element = sheets_element->InsertNewChildElement("sheet");
        sheet_element->SetAttribute("name", worksheet.name().c_str());
        sheet_element->SetAttribute("sheetId", worksheet.id());
        sheet_element->SetAttribute("state", "visible");
    }

    xml_document.InsertEndChild(workbook_element);

    auto printer = tinyxml2::XMLPrinter{};
    xml_document.Print(&printer);
    buffer.assign(printer.CStr(), printer.CStrSize());
}

auto serialize_worksheet(
    std ::string& buffer, tinyxml2::XMLDocument& xml_document, const xlsx::worksheet& worksheet)
{
    buffer.clear();
    xml_document.Clear();

    xml_document.InsertFirstChild(xml_document.NewDeclaration());

    auto* worksheet_element = xml_document.NewElement("worksheet");
    worksheet_element->SetAttribute(
        "xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    worksheet_element->SetAttribute(
        "xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");

    xml_document.InsertEndChild(worksheet_element);

    auto printer = tinyxml2::XMLPrinter{};
    xml_document.Print(&printer);
    buffer.assign(printer.CStr(), printer.CStrSize());
}

}   // namespace

namespace xlsx {

auto write(const workbook& workbook, const std::filesystem::path& filepath) -> expected<void>
{
    if (std::ranges::empty(workbook)) {
        return tl::make_unexpected("workbook empty");
    }
    if (filepath.empty()) {
        return tl::make_unexpected("file path empty");
    }

    const auto dir = filepath.parent_path() / filepath.stem();
    std::filesystem::create_directory(dir);

    const auto archive_path = dir / filepath.filename();
    auto maybe_zip_archive = detail::zip_archive::open(dir / filepath.filename());
    if (!maybe_zip_archive) {
        return tl::make_unexpected(maybe_zip_archive.error());
    }

    auto buffer = std::string{};
    auto xml_document = tinyxml2::XMLDocument{};
    serialize_workbook(buffer, xml_document, workbook);

    auto out_stream = std::ofstream{ dir / "workbook.xml" };
    out_stream << buffer;

    for (const auto& worksheet : workbook) {
        serialize_worksheet(buffer, xml_document, worksheet);
        auto out_sheet_stream = std::ofstream{ dir / fmt::format("sheet{}.xml", worksheet.id()) };
        out_sheet_stream << buffer;
    }

    return {};
}

}   // namespace xlsx
