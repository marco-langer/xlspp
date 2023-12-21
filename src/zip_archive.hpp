#pragma once

#include "xlspp/error.hpp"

#include <gsl/pointers>
#include <tl/expected.hpp>
#include <zip.h>

#include <filesystem>
#include <string>
#include <vector>

namespace xlsx::detail {

struct zip_file_info
{
    std::size_t index{};
    std::string filepath;
    bool encrypted{};
};

class zip_archive
{
public:
    using const_iterator = std::vector<zip_file_info>::const_iterator;

    auto begin() const& -> const_iterator;
    auto cbegin() const& -> const_iterator;
    auto end() const& -> const_iterator;
    auto cend() const& -> const_iterator;

    [[nodiscard]] static auto open(const std::filesystem::path& filepath) -> expected<zip_archive>;

    [[nodiscard]] auto contains(const std::string& relative_filepath) const -> bool;
    [[nodiscard]] auto read_all(const std::string& relative_filepath) const& -> const std::string*;

private:
    zip_archive(gsl::strict_not_null<zip*> za, std::vector<zip_file_info> file_infos);

    gsl::strict_not_null<zip*> m_za;
    std::vector<zip_file_info> m_file_infos;
    mutable std::string m_buffer;
};

}   // namespace xlsx::detail
