#pragma once

#include "xlspp/error.hpp"
#include "xlspp/workbook.hpp"

#include <filesystem>

namespace xlsx::detail {

class reader
{
public:
    auto read(const std::filesystem::path& filepath) -> expected<workbook>;

private:
};

}   // namespace xlsx::detail
