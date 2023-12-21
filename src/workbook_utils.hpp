#pragma once

#include "xlspp/workbook.hpp"

#include <tinyxml2.h>

namespace xlsx::detail {

auto parse_workbook(const tinyxml2::XMLDocument& xml_document) -> std::optional<workbook>;

}
