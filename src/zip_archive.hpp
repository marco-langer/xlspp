#pragma once

#include "xlspp/error.hpp"

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
    enum class open_mode
    {
        must_exist,
        create_if_not_exist
    };

    using const_iterator = std::vector<zip_file_info>::const_iterator;

    zip_archive(const zip_archive& other) = delete;
    zip_archive(zip_archive&& other) noexcept;
    zip_archive& operator=(const zip_archive& other) = delete;
    zip_archive& operator=(zip_archive&& other) noexcept;
    ~zip_archive();

    auto begin() const& -> const_iterator;
    auto cbegin() const& -> const_iterator;
    auto end() const& -> const_iterator;
    auto cend() const& -> const_iterator;

    [[nodiscard]] static auto
    open(const std::filesystem::path& filepath, open_mode open_mode = open_mode::must_exist)
        -> expected<zip_archive>;

    [[nodiscard]] auto contains(const std::string& relative_filepath) const -> bool;
    [[nodiscard]] auto read_all(const std::string& relative_filepath) const& -> const std::string*;

    auto add(std::string_view data, std::string relative_filepath) -> expected<void>;

private:
    zip_archive(zip* za, std::vector<zip_file_info> file_infos);

    zip_t* m_za{ nullptr };
    std::vector<zip_file_info> m_file_infos;
    mutable std::string m_buffer;
};

}   // namespace xlsx::detail
