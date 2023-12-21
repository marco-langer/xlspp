find_package(fmt CONFIG REQUIRED)
find_package(libzip CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

add_library(xlspp SHARED)

target_sources(xlspp
    PRIVATE
        include/xlspp/error.hpp
        include/xlspp/read.hpp
        include/xlspp/workbook.hpp
        include/xlspp/write.hpp
        include/xlspp/xlspp.hpp
        cell_type.hpp
        read.cpp
        reader.cpp
        reader.hpp
        workbook.cpp
        workbook_utils.cpp
        workbook_utils.hpp
        worksheet_utils.cpp
        worksheet_utils.hpp
        write.cpp
        zip_archive.cpp
        zip_archive.hpp
)

target_include_directories(xlspp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(xlspp
    PUBLIC
        tl::expected
    PRIVATE
        fmt::fmt
        libzip::zip
        Microsoft.GSL::GSL
        tinyxml2::tinyxml2
)


add_library(xlspp-test STATIC)

target_sources(xlspp-test
    PRIVATE
        include/xlspp/error.hpp
        include/xlspp/read.hpp
        include/xlspp/workbook.hpp
        include/xlspp/write.hpp
        include/xlspp/xlspp.hpp
        cell_type.hpp
        read.cpp
        reader.cpp
        reader.hpp
        workbook.cpp
        workbook_utils.cpp
        workbook_utils.hpp
        worksheet_utils.cpp
        worksheet_utils.hpp
        write.cpp
        zip_archive.cpp
        zip_archive.hpp
)

target_include_directories(xlspp-test
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(xlspp-test
    PUBLIC
        fmt::fmt
        libzip::zip
        Microsoft.GSL::GSL
        tinyxml2::tinyxml2
        tl::expected
)
