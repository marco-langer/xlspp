#include "worksheet_utils.hpp"

#include "cell_type.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>

namespace {

std::string str_tolower(std::string str)
{
    std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

}   // namespace

namespace xlsx::detail {

auto is_sheet_name_valid(std::string_view name) -> bool
{
    if (name.empty()) {
        return false;
    }

    constexpr auto invalid_chars = std::array{ '[', ']', '*', '?', ':', '/', '\\', '\'' };
    const char first = name.front();
    const auto found_first_pos = std::ranges::find(invalid_chars, first);
    if (found_first_pos != invalid_chars.end()) {
        return false;
    }

    const char last = name.back();
    const auto found_last_pos = std::ranges::find(invalid_chars, last);
    if (found_last_pos != invalid_chars.end()) {
        return false;
    }

    constexpr auto reserved_names = std::array{ "history" };
    const auto name_lowercase = str_tolower(std::string{ name });
    const auto found_reserved = std::ranges::find(reserved_names, name_lowercase);
    if (found_reserved != reserved_names.end()) {
        return false;
    }

    return true;
}

auto parse_worksheet(
    worksheet& worksheet,
    const tinyxml2::XMLDocument& xml_document,
    const std::vector<std::string>& shared_strings) -> bool
{
    const tinyxml2::XMLNode* xml_worksheet = xml_document.FirstChildElement("worksheet");
    if (!xml_worksheet) {
        return false;
    }
    const tinyxml2::XMLNode* sheetData = xml_worksheet->FirstChildElement("sheetData");
    if (!sheetData) {
        return false;
    }
    const tinyxml2::XMLNode* row = sheetData->FirstChildElement("row");
    while (row) {
        const tinyxml2::XMLNode* col = row->FirstChildElement("c");
        while (col) {
            const tinyxml2::XMLElement* col_element = col->ToElement();
            if (!col_element) {
                return false;
            }
            const char* cell_reference_attribute = col_element->Attribute("r");
            if (!cell_reference_attribute) {
                return false;
            }
            const std::string cell_reference{ cell_reference_attribute };
            const char* cell_type_attribute = col_element->Attribute("t");
            if (!cell_type_attribute) {
                return false;
            }
            const xlsx::detail::cell_type cell_type =
                xlsx::detail::to_cell_type(cell_type_attribute);
            const tinyxml2::XMLElement* cell_value = col->FirstChildElement("v");
            if (!cell_value) {
                return false;
            }
            const char* cell_text_value = cell_value->GetText();
            if (!cell_text_value) {
                return false;
            }
            const std::string_view cell_text{ cell_text_value };
            switch (cell_type) {
            case xlsx::detail::cell_type::Number: {
                auto number = std::stod(cell_text.data());
                // auto [_, ec] = std::from_chars(cell_text.data(), cell_text.data() +
                // cell_text.size(), number);
                worksheet.insert_or_assign(cell_reference, xlsx::cell{ cell_reference, number });
            } break;
            case xlsx::detail::cell_type::SharedString: {
                auto string_index = std::size_t{};
                auto [_, ec] = std::from_chars(
                    cell_text.data(), cell_text.data() + cell_text.size(), string_index);
                worksheet.insert_or_assign(
                    cell_reference, xlsx::cell{ cell_reference, &shared_strings.at(string_index) });
            } break;
            }
            col = col->NextSibling();
        }
        row = row->NextSibling();
    }

    return true;
}

}   // namespace xlsx::detail
