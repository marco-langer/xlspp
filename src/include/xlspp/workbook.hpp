#pragma once

#include "worksheet.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace xlsx {

namespace detail {
    class reader;
}

class workbook
{
public:
    using iterator = std::vector<worksheet>::iterator;
    using const_iterator = std::vector<worksheet>::const_iterator;

    [[nodiscard]] auto operator[](std::string_view name) & -> worksheet*;
    [[nodiscard]] auto operator[](std::string_view name) const& -> const worksheet*;

    [[nodiscard]] auto get_worksheet(std::string_view name) const& -> const worksheet&;

    [[nodiscard]] auto begin() & -> iterator;
    [[nodiscard]] auto begin() const& -> const_iterator;
    [[nodiscard]] auto cbegin() const& -> const_iterator;
    [[nodiscard]] auto end() & -> iterator;
    [[nodiscard]] auto end() const& -> const_iterator;
    [[nodiscard]] auto cend() const& -> const_iterator;

    auto application_name() const& -> const std::string&;
    auto set_application_name(std::string_view application_name) -> void;

    auto add_worksheet(std::string name) -> worksheet&;
    auto try_add_worksheet(std::string name) -> worksheet*;

    // non-public API, which will be removed when switching to C++20 modules
    auto _try_add_worksheet(int id, std::string name) -> worksheet*;

private:
    friend class detail::reader;

    int m_last_sheet_id{};
    std::vector<std::string> m_shared_strings;
    std::string m_application_name{ "xlspp " };
    std::vector<worksheet> m_worksheets;
};

}   // namespace xlsx
