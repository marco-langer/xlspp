#include "xlspp/write.hpp"

#include "workbook_utils.hpp"
#include "worksheet_utils.hpp"
#include "zip_archive.hpp"

#include <fmt/core.h>
#include <tinyxml2.h>

#include <algorithm>
#include <string>
#include <vector>

namespace {

auto serialize_content_type(const xlsx::workbook& workbook, tinyxml2::XMLDocument& xml_document)
    -> std::string
{
    xml_document.Clear();

    xml_document.InsertFirstChild(xml_document.NewDeclaration());

    auto* types_element = xml_document.NewElement("Types");
    types_element->SetAttribute(
        "xmlns", "http://schemas.openxmlformats.org/package/2006/content-types");

    auto* default_element1 = types_element->InsertNewChildElement("Default");
    default_element1->SetAttribute("Extension", "rels");
    default_element1->SetAttribute(
        "ContentType", "application/vnd.openxmlformats-package.relationships+xml");

    auto* default_element2 = types_element->InsertNewChildElement("Default");
    default_element2->SetAttribute("Extension", "xml");
    default_element2->SetAttribute("ContentType", "application/xml");

    auto* override_workbook_element = types_element->InsertNewChildElement("Override");
    override_workbook_element->SetAttribute("PartName", "/xl/workbook.xml");
    override_workbook_element->SetAttribute(
        "ContentType",
        "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml");

    for (const auto& worksheet : workbook) {
        auto* override_worksheet_element = types_element->InsertNewChildElement("Override");
        override_worksheet_element->SetAttribute(
            "PartName", fmt::format("/xl/worksheets/sheet{}.xml", worksheet.id()).c_str());
        override_worksheet_element->SetAttribute(
            "ContentType",
            "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml");
    }

    xml_document.InsertEndChild(types_element);

    auto printer = tinyxml2::XMLPrinter{ nullptr, true, 0 };
    xml_document.Print(&printer);
    return std::string{ printer.CStr(), static_cast<std::size_t>(printer.CStrSize() - 1) };
}

auto serialize_relationships(tinyxml2::XMLDocument& xml_document) -> std::string
{
    xml_document.Clear();

    xml_document.InsertFirstChild(xml_document.NewDeclaration());

    auto* relationships_element = xml_document.NewElement("Relationships");
    relationships_element->SetAttribute(
        "xmlns", "http://schemas.openxmlformats.org/package/2006/relationships");

    auto* relationship_element = relationships_element->InsertNewChildElement("Relationship");
    relationship_element->SetAttribute("Id", "rId1");
    relationship_element->SetAttribute(
        "Type",
        "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument");
    relationship_element->SetAttribute("Target", "xl/workbook.xml");

    xml_document.InsertEndChild(relationships_element);

    auto printer = tinyxml2::XMLPrinter{ nullptr, true, 0 };
    xml_document.Print(&printer);
    return std::string{ printer.CStr(), static_cast<std::size_t>(printer.CStrSize() - 1) };
}

}   // namespace

namespace xlsx {

auto write(const workbook& workbook, const std::filesystem::path& filepath) -> expected<void>
{
    if (std::ranges::empty(workbook)) {
        return tl::make_unexpected("workbook empty");
    }
    if (filepath.empty()) {
        return tl::make_unexpected("filepath empty");
    }

    auto buffers = std::vector<std::string>{};
    auto maybe_zip_archive =
        detail::zip_archive::open(filepath, detail::zip_archive::open_mode::create_if_not_exist);
    if (!maybe_zip_archive) {
        return tl::make_unexpected(maybe_zip_archive.error());
    }
    auto& zip_archive = maybe_zip_archive.value();

    auto xml_document = tinyxml2::XMLDocument{};
    auto& content_type_buffer =
        buffers.emplace_back(serialize_content_type(workbook, xml_document));
    zip_archive.add(content_type_buffer, "[Content_Types].xml");
    auto& relationships_buffer = buffers.emplace_back(serialize_relationships(xml_document));
    zip_archive.add(relationships_buffer, "_rels/.rels");
    auto& workbook_buffer =
        buffers.emplace_back(detail::serialize_workbook(workbook, xml_document));
    zip_archive.add(workbook_buffer, "xl/workbook.xml");

    for (const auto& worksheet : workbook) {
        auto& worksheet_buffer =
            buffers.emplace_back(detail::serialize_worksheet(worksheet, {}, xml_document));
        zip_archive.add(worksheet_buffer, fmt::format("xl/worksheets/sheet{}.xml", worksheet.id()));
    }

    return {};
}

}   // namespace xlsx
