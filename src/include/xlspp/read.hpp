#pragma once

#include "error.hpp"
#include "workbook.hpp"

#include <filesystem>

namespace xlsx {

[[nodiscard]] auto read(const std::filesystem::path& filepath) -> expected<workbook>;

}   // namespace xlsx
