#pragma once

#include "workbook.hpp"

#include <filesystem>
#include <optional>

namespace xlsx {

auto read(const std::filesystem::path& filepath) -> std::optional<workbook>;

}
