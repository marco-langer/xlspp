#include "xlspp/xlspp.hpp"

#include <fmt/core.h>

int main()
{
    xlsx::workbook workbook;
    workbook.add_worksheet("Sheet1");

    if (const xlsx::write_result result = xlsx::write(workbook, "hello_world.xlsx"); !result) {
        fmt::print("error: {}\n", result.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
