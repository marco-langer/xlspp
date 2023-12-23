enable_testing()

find_package(GTest CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

add_executable(xlspp-tests)

target_sources(xlspp-tests
    PRIVATE
        test_read.cpp
        test_read_invalid.cpp
        test_workbook_utils.cpp
        test_worksheet.cpp
        test_write.cpp
        test_write_invalid.cpp
        test_zip_archive.cpp
)

target_link_libraries(xlspp-tests
    PRIVATE
        xlspp-static
        GTest::gtest_main
        range-v3::range-v3
)

include(GoogleTest)
gtest_discover_tests(xlspp-tests)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
