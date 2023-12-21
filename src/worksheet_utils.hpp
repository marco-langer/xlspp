#pragma once

#include "xlspp/worksheet.hpp"

#include <tinyxml2.h>

#include <string_view>
#include <vector>

namespace xlsx::detail {

auto is_sheet_name_valid(std::string_view name) -> bool;

auto parse_worksheet(
    worksheet& worksheet,
    const tinyxml2::XMLDocument& xml_document,
    const std::vector<std::string>& shared_strings) -> bool;

}   // namespace xlsx::detail
