#pragma once

#include <string>
#include <variant>

namespace xlsx {

class cell
{
public:
    using cell_data = std::variant<const std::string*, bool, double>;

    explicit cell(std::string reference, cell_data value)
        : m_reference{ std::move(reference) }
        , m_value{ std::move(value) }
    {}

    auto is_boolean() const -> bool { return std::holds_alternative<bool>(m_value); }

    auto if_boolean() const -> const bool* { return std::get_if<bool>(&m_value); }

    auto is_number() const -> bool { return std::holds_alternative<double>(m_value); }

    auto if_number() const -> const double* { return std::get_if<double>(&m_value); }

private:
    std::string m_reference;
    cell_data m_value;
};

}   // namespace xlsx
