#pragma once

namespace xlsx::detail {

enum class cell_type
{
    Boolean,
    Number,
    Error,
    SharedString,
    String,
    InlineString,
    Date
};

cell_type to_cell_type(std::string_view type)
{
    if (type == "n") {
        return cell_type::Number;
    }

    if (type == "s") {
        return cell_type::SharedString;
    }

    if (type == "str") {
        return cell_type::String;
    }

    if (type == "inlineStr") {
        return cell_type::InlineString;
    }

    if (type == "b") {
        return cell_type::Boolean;
    }

    if (type == "e") {
        return cell_type::Error;
    }

    if (type == "d") {
        return cell_type::Date;
    }

    return cell_type::Error;
}

}   // namespace xlsx::detail
