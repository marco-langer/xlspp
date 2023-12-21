#pragma once

#include <tl/expected.hpp>

#include <string>

namespace xlsx {

template <typename T>
using expected = tl::expected<T, std::string>;

}
