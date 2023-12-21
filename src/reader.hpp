#pragma once

#include "xlspp/workbook.hpp"

#include <filesystem>

namespace xlsx::detail {

class reader
{
public:
    auto read(const std::filesystem::path& filepath) -> std::optional<workbook>;

private:
};

}   // namespace xlsx::detail
