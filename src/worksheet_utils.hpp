#pragma once

#include "xlspp/error.hpp"
#include "xlspp/worksheet.hpp"

#include <tinyxml2.h>

#include <string>
#include <string_view>
#include <vector>

namespace xlsx::detail {

auto is_sheet_name_valid(std::string_view name) -> bool;

auto parse_worksheet(
    worksheet& worksheet,
    const std::vector<std::string>& shared_strings,
    const tinyxml2::XMLDocument& xml_document) -> expected<void>;

auto serialize_worksheet(
    const worksheet& worksheet,
    const std::vector<std::string>& shared_strings,
    tinyxml2::XMLDocument& xml_document) -> std::string;

}   // namespace xlsx::detail
