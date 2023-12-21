#pragma once

#include "error.hpp"
#include "workbook.hpp"

#include <filesystem>

namespace xlsx {

using write_result = expected<void>;

[[nodiscard]] auto write(const workbook& workbook, const std::filesystem::path& filepath)
    -> write_result;

}   // namespace xlsx
