#include "xlspp/read.hpp"

#include "reader.hpp"

namespace xlsx {

auto read(const std::filesystem::path& filepath) -> expected<workbook>
{
    return detail::reader{}.read(filepath);
}

}   // namespace xlsx
