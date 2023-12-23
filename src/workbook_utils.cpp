#include "workbook_utils.hpp"

#include <charconv>
#include <string_view>

namespace xlsx::detail {

auto parse_workbook(const tinyxml2::XMLDocument& xml_document) -> expected<workbook>
{
    const auto* workbook_element = xml_document.FirstChildElement("workbook");
    if (!workbook_element) {
        return tl::make_unexpected("workbook element not found");
    }

    auto result_workbook = workbook{};
    const auto* file_version_element = workbook_element->FirstChildElement("fileVersion");
    if (!file_version_element) {
        return tl::make_unexpected("fileVersion element not found");
    }
    const auto* app_name_cstr = file_version_element->Attribute("appName");
    if (!app_name_cstr) {
        return tl::make_unexpected("appName attribute not found");
    }
    result_workbook.set_application_name(app_name_cstr);

    const auto* sheets_element = workbook_element->FirstChildElement("sheets");
    if (!sheets_element) {
        return tl::make_unexpected("sheets element not found");
    }
    const auto* sheet_element = sheets_element->FirstChildElement("sheet");
    while (sheet_element) {
        if (!sheet_element) {
            return tl::make_unexpected("sheet element not found");
        }

        const auto* sheet_name = sheet_element->Attribute("name");
        if (!sheet_name) {
            return tl::make_unexpected("sheet name attribute not found");
        }

        const auto* sheet_id_str = sheet_element->Attribute("sheetId");
        if (!sheet_id_str) {
            return tl::make_unexpected("sheetId attribute not found");
        }
        auto sheet_id = int{};
        auto [_, error_code] =
            std::from_chars(sheet_id_str, sheet_id_str + std::strlen(sheet_id_str), sheet_id);
        if (error_code != std::errc{}) {
            return tl::make_unexpected("sheetId attribute not a number");
        }

        auto* worksheet = result_workbook._try_add_worksheet(sheet_id, sheet_name);
        if (!worksheet) {
            return tl::make_unexpected("sheet name/id duplicate or invalid attribute not a number");
        }

        const auto* sheet_state_cstr = sheet_element->Attribute("state");
        if (sheet_state_cstr) {
            const auto sheet_state = std::string_view{ sheet_state_cstr };
            if (sheet_state == "hidden") {
                worksheet->set_visible(false);
            } else if (sheet_state != "visible") {
                // TODO what other states are there except visible/hidden?
                return tl::make_unexpected("unexpected sheet attribute");
            }
        }

        sheet_element = sheet_element->NextSiblingElement();
    }

    return std::move(result_workbook);
}

auto serialize_workbook(const workbook& workbook, tinyxml2::XMLDocument& xml_document)
    -> std::string
{
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

    auto printer = tinyxml2::XMLPrinter{ nullptr, true, 0 };
    xml_document.Print(&printer);
    return std::string{ printer.CStr(), static_cast<std::size_t>(printer.CStrSize() - 1) };
}

}   // namespace xlsx::detail
