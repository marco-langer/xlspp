#include "xlspp/xlspp.hpp"

#include <fmt/core.h>

int main()
{
    xlsx::workbook workbook;
    workbook.set_application_name("my app");

    xlsx::worksheet& sheet1 = workbook.add_worksheet("Sheet1");

    xlsx::worksheet& sheet2 = workbook.add_worksheet("Sheet2");
    sheet2.set_visible(false);

    if (const xlsx::write_result result = xlsx::write(workbook, "hello_world_extended.xlsx");
        !result) {
        fmt::print("error: {}\n", result.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
