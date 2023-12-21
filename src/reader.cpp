#include "reader.hpp"

#include "workbook_utils.hpp"
#include "worksheet_utils.hpp"
#include "zip_archive.hpp"

#include <fmt/core.h>
#include <tinyxml2.h>

namespace {

auto parse_shared_strings(const tinyxml2::XMLDocument& xml_document)
    -> std::optional<std::vector<std::string>>
{
    auto shared_strings = std::vector<std::string>{};

    const tinyxml2::XMLNode* sst = xml_document.FirstChildElement("sst");
    if (!sst) {
        return std::nullopt;
    }
    const tinyxml2::XMLNode* si = sst->FirstChild();
    while (si) {
        const tinyxml2::XMLNode* t = si->FirstChild();
        if (!t) {
            return std::nullopt;
        }
        const tinyxml2::XMLNode* tText = t->FirstChild();
        if (!tText) {
            return std::nullopt;
        }
        const char* text = tText->Value();
        if (!text) {
            return std::nullopt;
        }
        shared_strings.emplace_back(text);
        si = si->NextSibling();
    }

    return shared_strings;
}

auto read_shared_strings(
    xlsx::detail::zip_archive& zip_archive, tinyxml2::XMLDocument& xml_document)
    -> std::optional<std::vector<std::string>>
{
    const auto shared_strings_path = std::string{ "xl/sharedStrings.xml" };
    if (!zip_archive.contains(shared_strings_path)) {
        return std::vector<std::string>{};
    }

    const auto* read_buffer = zip_archive.read_all(shared_strings_path);
    if (!read_buffer) {
        return std::nullopt;
    }
    xml_document.Parse(read_buffer->c_str());

    return parse_shared_strings(xml_document);
}

auto read_workbook(xlsx::detail::zip_archive& zip_archive, tinyxml2::XMLDocument& xml_document)
    -> std::optional<xlsx::workbook>
{
    const auto* read_buffer = zip_archive.read_all("xl/workbook.xml");
    if (!read_buffer) {
        return std::nullopt;
    }
    xml_document.Parse(read_buffer->c_str());

    return xlsx::detail::parse_workbook(xml_document);
}

auto read_worksheets(
    xlsx::workbook& workbook,
    xlsx::detail::zip_archive& zip_archive,
    tinyxml2::XMLDocument& xml_document,
    const std::vector<std::string>& shared_strings) -> bool
{
    auto filepath = std::string{};
    for (auto& worksheet : workbook) {
        filepath.clear();
        fmt::format_to(std::back_inserter(filepath), "xl/worksheets/sheet{}.xml", worksheet.id());

        const auto* read_buffer = zip_archive.read_all(filepath);
        if (!read_buffer) {
            return false;
        }
        // TODO handle error
        const auto xml_error = xml_document.Parse(read_buffer->c_str());

        if (!xlsx::detail::parse_worksheet(worksheet, xml_document, shared_strings)) {
            return false;
        }
    }

    return true;
}

}   // namespace


namespace xlsx::detail {

auto reader::read(const std::filesystem::path& filepath) -> std::optional<workbook>
{
    auto maybe_zip_archive = detail::zip_archive::open(filepath);
    if (!maybe_zip_archive.has_value()) {
        return std::nullopt;
    }
    auto& zip_archive = maybe_zip_archive.value();

    auto xml_document = tinyxml2::XMLDocument{};
    auto maybe_shared_strings = read_shared_strings(zip_archive, xml_document);
    if (!maybe_shared_strings.has_value()) {
        return std::nullopt;
    }
    auto& shared_strings = maybe_shared_strings.value();

    auto maybe_workbook = read_workbook(zip_archive, xml_document);
    if (!maybe_workbook.has_value()) {
        return std::nullopt;
    }
    auto& workbook = maybe_workbook.value();
    if (!read_worksheets(workbook, zip_archive, xml_document, shared_strings)) {
        return std::nullopt;
    }

    workbook.m_shared_strings = std::move(shared_strings);

    return maybe_workbook;
}

}   // namespace xlsx::detail
