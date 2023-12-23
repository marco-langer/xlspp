#pragma once

#include "xlspp/error.hpp"
#include "xlspp/workbook.hpp"

#include <tinyxml2.h>

#include <string>

namespace xlsx::detail {

auto parse_workbook(const tinyxml2::XMLDocument& xml_document) -> expected<workbook>;

auto serialize_workbook(const workbook& workbook, tinyxml2::XMLDocument& xml_document)
    -> std::string;

}   // namespace xlsx::detail
