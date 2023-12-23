#include "reader.hpp"

#include "workbook_utils.hpp"
#include "worksheet_utils.hpp"
#include "zip_archive.hpp"

#include <fmt/core.h>
#include <tinyxml2.h>

#include <string>
#include <vector>

namespace {

auto parse_shared_strings(const tinyxml2::XMLDocument& xml_document)
    -> xlsx::expected<std::vector<std::string>>
{
    auto shared_strings = std::vector<std::string>{};

    const auto* sst_element = xml_document.FirstChildElement("sst");
    if (!sst_element) {
        return tl::make_unexpected("sst element not found");
    }
    const auto* si = sst_element->FirstChild();
    while (si) {
        const tinyxml2::XMLNode* t = si->FirstChild();
        if (!t) {
            return tl::make_unexpected("t node not found");
        }
        const tinyxml2::XMLNode* tText = t->FirstChild();
        if (!tText) {
            return tl::make_unexpected("tText node not found");
        }
        const char* text = tText->Value();
        if (!text) {
            return tl::make_unexpected("text not found");
        }
        shared_strings.emplace_back(text);
        si = si->NextSibling();
    }

    return std::move(shared_strings);
}

auto read_shared_strings(
    xlsx::detail::zip_archive& zip_archive, tinyxml2::XMLDocument& xml_document)
    -> xlsx::expected<std::vector<std::string>>
{
    const auto shared_strings_path = std::string{ "xl/sharedStrings.xml" };
    if (!zip_archive.contains(shared_strings_path)) {
        return {};
    }

    const auto* read_buffer = zip_archive.read_all(shared_strings_path);
    if (!read_buffer) {
        return tl::make_unexpected(
            fmt::format("file '{}' not found in archive.", shared_strings_path));
    }
    xml_document.Parse(read_buffer->c_str());
    if (xml_document.Error()) {
        return tl::make_unexpected(xml_document.ErrorStr());
    }

    return parse_shared_strings(xml_document);
}

auto read_workbook(xlsx::detail::zip_archive& zip_archive, tinyxml2::XMLDocument& xml_document)
    -> xlsx::expected<xlsx::workbook>
{
    const auto* read_buffer = zip_archive.read_all("xl/workbook.xml");
    if (!read_buffer) {
        return tl::make_unexpected("xl/workbook.xml file not found");
    }
    xml_document.Parse(read_buffer->c_str());
    if (xml_document.Error()) {
        return tl::make_unexpected("error while parsing xl/workbook.xml");
    }
    return xlsx::detail::parse_workbook(xml_document);
}

auto read_worksheets(
    xlsx::workbook& workbook,
    xlsx::detail::zip_archive& zip_archive,
    tinyxml2::XMLDocument& xml_document,
    const std::vector<std::string>& shared_strings) -> xlsx::expected<void>
{
    auto filepath = std::string{};
    for (auto& worksheet : workbook) {
        filepath.clear();
        fmt::format_to(std::back_inserter(filepath), "xl/worksheets/sheet{}.xml", worksheet.id());

        const auto* read_buffer = zip_archive.read_all(filepath);
        if (!read_buffer) {
            return tl::make_unexpected(fmt::format("file not found: {}", filepath));
        }
        xml_document.Parse(read_buffer->c_str());
        if (xml_document.Error()) {
            return tl::make_unexpected(xml_document.ErrorStr());
        }
        if (auto result = xlsx::detail::parse_worksheet(worksheet, shared_strings, xml_document);
            !result) {
            return result;
        }
    }

    return {};
}

}   // namespace


namespace xlsx::detail {

auto reader::read(const std::filesystem::path& filepath) -> expected<workbook>
{
    auto maybe_zip_archive = detail::zip_archive::open(filepath);
    if (!maybe_zip_archive.has_value()) {
        return tl::make_unexpected(maybe_zip_archive.error());
    }
    auto& zip_archive = maybe_zip_archive.value();

    auto xml_document = tinyxml2::XMLDocument{};
    auto maybe_shared_strings = read_shared_strings(zip_archive, xml_document);
    if (!maybe_shared_strings.has_value()) {
        return tl::make_unexpected(maybe_shared_strings.error());
    }
    auto& shared_strings = maybe_shared_strings.value();

    auto maybe_workbook = read_workbook(zip_archive, xml_document);
    if (!maybe_workbook.has_value()) {
        return maybe_workbook;
    }

    auto& workbook = maybe_workbook.value();
    if (auto result = read_worksheets(workbook, zip_archive, xml_document, shared_strings);
        !result) {
        return tl::make_unexpected(std::move(result.error()));
    }

    workbook.m_shared_strings = std::move(shared_strings);

    return maybe_workbook;
}

}   // namespace xlsx::detail
