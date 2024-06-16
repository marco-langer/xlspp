#pragma once

#include "cell.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace xlsx {

class worksheet
{
public:
    enum class visibility_state { visible, hidden, very_hidden };

    explicit worksheet(int id, std::string name)
        : m_id{ id }
        , m_name{ std::move(name) }
    {}

    worksheet(const worksheet& other) = delete;
    worksheet(worksheet&& other) = default;
    worksheet& operator=(const worksheet& other) = delete;
    worksheet& operator=(worksheet&& other) = default;

    auto id() const -> std::size_t { return m_id; }

    auto name() const -> const std::string& { return m_name; }

    auto visibility() const -> visibility_state;
    auto set_visibility(visibility_state visibility) { m_visibility = visibility; }
    auto visible() const -> bool { return m_visibility == visibility_state::visible; }
    auto set_visible(bool visible) -> void { m_visibility = visibility_state::visible; };

    auto cell_count() const -> std::size_t { return m_cells.size(); }

    auto operator[](const std::string& reference) const -> const cell*
    {
        auto cell_it = m_cells.find(reference);
        return cell_it != m_cells.end() ? &cell_it->second : nullptr;
    }

    auto at(const std::string& reference) const -> const cell&
    {
        auto cell_it = m_cells.find(reference);
        if (cell_it == m_cells.end()) {
            throw std::invalid_argument{ "TODO" };
        }
        return cell_it->second;
    }

    auto insert_or_assign(const std::string& reference, cell cell) -> void
    {
        m_cells.insert_or_assign(reference, std::move(cell));
    }

private:
    friend class workbook;

    int m_id{};
    std::string m_name;
    visibility_state m_visibility{ visibility_state::visible };
    std::unordered_map<std::string, cell> m_cells;
};

}   // namespace xlsx
