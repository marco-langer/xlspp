#include "xlspp/read.hpp"

#include "reader.hpp"

namespace xlsx {

auto read(const std::filesystem::path& filepath) -> std::optional<workbook>
{
    return detail::reader{}.read(filepath);
}

}   // namespace xlsx
