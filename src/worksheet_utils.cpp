#include "worksheet_utils.hpp"

#include "cell_type.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>

namespace {

auto str_tolower(std::string str) -> std::string
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
    const std::vector<std::string>& shared_strings,
    const tinyxml2::XMLDocument& xml_document) -> expected<void>
{
    const auto* xml_worksheet = xml_document.FirstChildElement("worksheet");
    if (!xml_worksheet) {
        return tl::make_unexpected("worksheet element not found");
    }
    const auto* sheetData_node = xml_worksheet->FirstChildElement("sheetData");
    if (!sheetData_node) {
        return tl::make_unexpected("sheetData node not found");
    }

    const tinyxml2::XMLNode* row_node = sheetData_node->FirstChildElement("row");
    while (row_node) {
        const tinyxml2::XMLNode* col_node = row_node->FirstChildElement("c");
        while (col_node) {
            const auto* col_element = col_node->ToElement();
            if (!col_element) {
                return tl::make_unexpected("col node not an element");
            }
            const char* cell_reference_attribute = col_element->Attribute("r");
            if (!cell_reference_attribute) {
                return tl::make_unexpected("r attribute not found");
            }
            const std::string cell_reference{ cell_reference_attribute };
            const char* cell_type_attribute = col_element->Attribute("t");
            if (!cell_type_attribute) {
                return tl::make_unexpected("t attribute not found");
            }
            const xlsx::detail::cell_type cell_type =
                xlsx::detail::to_cell_type(cell_type_attribute);
            const tinyxml2::XMLElement* cell_value = col_element->FirstChildElement("v");
            if (!cell_value) {
                return tl::make_unexpected("v attribute not found");
            }
            const char* cell_text_value = cell_value->GetText();
            if (!cell_text_value) {
                return tl::make_unexpected("v attribute not a text");
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
            col_node = col_node->NextSibling();
        }
        row_node = row_node->NextSibling();
    }

    return {};
}

auto serialize_worksheet(
    const worksheet& worksheet,
    const std::vector<std::string>& shared_strings,
    tinyxml2::XMLDocument& xml_document) -> std::string
{
    xml_document.Clear();

    xml_document.InsertFirstChild(xml_document.NewDeclaration());

    auto* worksheet_element = xml_document.NewElement("worksheet");
    worksheet_element->SetAttribute(
        "xmlns", "http://schemas.openxmlformats.org/spreadsheetml/2006/main");
    worksheet_element->SetAttribute(
        "xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");

    worksheet_element->InsertNewChildElement("sheetData");

    xml_document.InsertEndChild(worksheet_element);

    auto printer = tinyxml2::XMLPrinter{ nullptr, true, 0 };
    xml_document.Print(&printer);
    return std::string{ printer.CStr(), static_cast<std::size_t>(printer.CStrSize() - 1) };
}

}   // namespace xlsx::detail
