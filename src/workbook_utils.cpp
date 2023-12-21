#include "workbook_utils.hpp"

#include <charconv>
#include <string_view>

namespace xlsx::detail {

auto parse_workbook(const tinyxml2::XMLDocument& xml_document) -> std::optional<workbook>
{
    const auto* workbook_node = xml_document.FirstChildElement("workbook");
    if (!workbook_node) {
        return std::nullopt;
    }

    auto result_workbook = workbook{};

    const auto* file_version_node = workbook_node->FirstChildElement("fileVersion");
    if (!file_version_node) {
        return std::nullopt;
    }
    const auto* file_version_element = file_version_node->ToElement();
    if (!file_version_element) {
        return std::nullopt;
    }
    const auto* app_name_cstr = file_version_element->Attribute("appName");
    if (!app_name_cstr) {
        return std::nullopt;
    }
    result_workbook.set_application_name(app_name_cstr);

    const auto* sheets_element = workbook_node->FirstChildElement("sheets");
    if (!sheets_element) {
        return std::nullopt;
    }
    const auto* sheet_element = sheets_element->FirstChildElement("sheet");
    while (sheet_element) {
        if (!sheet_element) {
            return std::nullopt;
        }

        const auto* sheet_name = sheet_element->Attribute("name");
        if (!sheet_name) {
            return std::nullopt;
        }

        const auto* sheet_id_str = sheet_element->Attribute("sheetId");
        if (!sheet_id_str) {
            return std::nullopt;
        }
        auto sheet_id = int{};
        auto [_, error_code] =
            std::from_chars(sheet_id_str, sheet_id_str + std::strlen(sheet_id_str), sheet_id);
        if (error_code != std::errc{}) {
            return std::nullopt;
        }

        auto* worksheet = result_workbook._try_add_worksheet(sheet_id, sheet_name);
        if (!worksheet) {
            return std::nullopt;
        }

        const auto* sheet_state_cstr = sheet_element->Attribute("state");
        if (sheet_state_cstr) {
            const auto sheet_state = std::string_view{ sheet_state_cstr };
            if (sheet_state == "hidden") {
                worksheet->set_visible(false);
            } else if (sheet_state != "visible") {
                // TODO what other states are there except visible/hidden?
                return std::nullopt;
            }
        }

        sheet_element = sheet_element->NextSiblingElement();
    }

    return result_workbook;
}

}   // namespace xlsx::detail
