#include "xlspp/workbook.hpp"

#include "worksheet_utils.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace xlsx {

auto workbook::operator[](std::string_view name) & -> worksheet*
{
    auto worksheet = std::ranges::find(m_worksheets, name, &worksheet::name);
    return worksheet != m_worksheets.end() ? &*worksheet : nullptr;
}

auto workbook::operator[](std::string_view name) const& -> const worksheet*
{
    const auto worksheet = std::ranges::find(m_worksheets, name, &worksheet::name);
    return worksheet != m_worksheets.end() ? &*worksheet : nullptr;
}

auto workbook::get_worksheet(std::string_view name) const& -> const worksheet&
{
    const auto worksheet = std::ranges::find(m_worksheets, name, &worksheet::name);
    if (worksheet == m_worksheets.end()) {
        throw std::invalid_argument{ "TODO" };
    }
    return *worksheet;
}

auto workbook::begin() & -> iterator { return m_worksheets.begin(); }
auto workbook::begin() const& -> const_iterator { return m_worksheets.begin(); }
auto workbook::cbegin() const& -> const_iterator { return m_worksheets.cbegin(); }
auto workbook::end() & -> iterator { return m_worksheets.end(); }
auto workbook::end() const& -> const_iterator { return m_worksheets.end(); }
auto workbook::cend() const& -> const_iterator { return m_worksheets.end(); }

auto workbook::application_name() const& -> const std::string& { return m_application_name; }

auto workbook::set_application_name(std::string_view application_name) -> void
{
    m_application_name = application_name;
}

auto workbook::add_worksheet(std::string name) -> worksheet&
{
    auto* added_worksheet = _try_add_worksheet(m_last_sheet_id + 1, std::move(name));
    if (!added_worksheet) {
        throw std::invalid_argument{ "TODO" };
    }
    return *added_worksheet;
}

auto workbook::try_add_worksheet(std::string name) -> worksheet*
{
    return _try_add_worksheet(m_last_sheet_id + 1, std::move(name));
}

auto workbook::_try_add_worksheet(int id, std::string name) -> worksheet*
{
    const auto worksheet_iter =
        std::ranges::find_if(m_worksheets, [id, &name](const worksheet& worksheet) {
            return worksheet.m_id == id || worksheet.name() == name;
        });
    if (worksheet_iter != m_worksheets.end()) {
        return nullptr;
    }
    if (!detail::is_sheet_name_valid(name)) {
        return nullptr;
    }

    auto& added_worksheet = m_worksheets.emplace_back(id, std::move(name));
    const auto max_id = std::ranges::max(m_worksheets | std::views::transform(&worksheet::id));
    m_last_sheet_id = max_id + 1;

    return &added_worksheet;
}

}   // namespace xlsx
