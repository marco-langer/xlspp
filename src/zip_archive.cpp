#include "zip_archive.hpp"

#include <algorithm>

namespace {

auto get_file_indices(zip* za) -> xlsx::expected<std::vector<xlsx::detail::zip_file_info>>
{
    // TODO second param?
    const auto num_entries = zip_get_num_entries(za, 0);
    if (num_entries < 0) {
        return tl::make_unexpected("error reading num entries");
    }

    auto file_infos = std::vector<xlsx::detail::zip_file_info>{};
    file_infos.reserve(static_cast<std::size_t>(num_entries));
    struct zip_stat file_stats;
    zip_stat_init(&file_stats);
    for (auto i = 0LL; i < num_entries; ++i) {
        // TODO third param?
        const auto result = zip_stat_index(za, i, 0, &file_stats);
        constexpr auto all_valid = 255;
        if (result == 0 && file_stats.name && file_stats.valid == all_valid) {
            file_infos.emplace_back(
                file_stats.index, file_stats.name, file_stats.encryption_method != ZIP_EM_NONE);
        } else {
            // TODO how to get the error from zip_stat_index() ?
            return tl::make_unexpected("error while enumerating files in xlsx archive");
        }
    }

    return std::move(file_infos);
}

}   // namespace

namespace xlsx::detail {

zip_archive::zip_archive(zip_archive&& other) noexcept
    : m_za{ std::exchange(other.m_za, nullptr) }
    , m_file_infos{ std::move(other.m_file_infos) }
    , m_buffer{ std::move(other.m_buffer) }
{}

zip_archive& zip_archive::operator=(zip_archive&& other) noexcept
{
    m_za = std::exchange(other.m_za, nullptr);
    return *this;
}

zip_archive::~zip_archive()
{
    if (m_za) {
        zip_close(m_za);
    }
}

auto zip_archive::begin() const& -> const_iterator { return m_file_infos.begin(); }
auto zip_archive::cbegin() const& -> const_iterator { return m_file_infos.cbegin(); }
auto zip_archive::end() const& -> const_iterator { return m_file_infos.end(); }
auto zip_archive::cend() const& -> const_iterator { return m_file_infos.cend(); }

auto zip_archive::open(const std::filesystem::path& filepath, open_mode open_mode)
    -> expected<zip_archive>
{
    auto error_code = int{};
    const auto open_flag = (open_mode == open_mode::must_exist) ? 0 : ZIP_CREATE;
    auto* za = zip_open(filepath.string().c_str(), open_flag, &error_code);
    if (!za) {
        auto error = zip_error_t{};
        zip_error_init_with_code(&error, error_code);
        zip_error_strerror(&error);
        return tl::make_unexpected(zip_error_strerror(&error));
    }
    auto maybe_file_indices = get_file_indices(za);
    if (!maybe_file_indices) {
        return tl::make_unexpected(maybe_file_indices.error());
    }

    return zip_archive{ za, std::move(maybe_file_indices.value()) };
}

auto zip_archive::contains(const std::string& relative_filepath) const -> bool
{
    return m_file_infos.end()
        != std::ranges::find(m_file_infos, relative_filepath, &zip_file_info::filepath);
}

auto zip_archive::read_all(const std::string& relative_filepath) const& -> const std::string*
{
    // TODO error handling
    const auto index_iter =
        std::ranges::find(m_file_infos, relative_filepath, &zip_file_info::filepath);
    if (index_iter == m_file_infos.end()) {
        return nullptr;
    }

    auto* zf = zip_fopen_index(m_za, index_iter->index, 0);
    if (!zf) {
        return nullptr;
    }

    m_buffer.clear();
    constexpr auto buffer_size = 1024ULL;
    char buffer[buffer_size];
    auto file_content = std::string{};
    auto bytes_read = zip_fread(zf, buffer, buffer_size);
    while (bytes_read > 0) {
        m_buffer.append(buffer, bytes_read);
        bytes_read = zip_fread(zf, buffer, buffer_size);
    }

    // TODO what to do with the error code?
    [[maybe_unused]] const auto errc = zip_fclose(zf);

    return &m_buffer;
}

auto zip_archive::add(std::string_view data, std::string relative_filepath) -> expected<void>
{
    if (relative_filepath.empty()) {
        return tl::make_unexpected("filepath empty");
    }

    auto* source = zip_source_buffer(m_za, data.data(), data.size(), 0);
    if (!source) {
        return tl::make_unexpected(zip_strerror(m_za));
    }
    const auto added_index =
        zip_file_add(m_za, relative_filepath.c_str(), source, ZIP_FL_ENC_UTF_8);
    if (added_index < 0) {
        return tl::make_unexpected(zip_strerror(m_za));
    }

    m_file_infos.push_back(zip_file_info{ .index = static_cast<std::size_t>(added_index),
                                          .filepath = std::move(relative_filepath) });
    return {};
}

zip_archive::zip_archive(zip* za, std::vector<zip_file_info> file_infos)
    : m_za{ za }
    , m_file_infos{ std::move(file_infos) }
{}

}   // namespace xlsx::detail
