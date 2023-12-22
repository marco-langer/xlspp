find_package(fmt CONFIG REQUIRED)
find_package(libzip CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

add_library(xlspp-obj OBJECT)

target_sources(xlspp-obj
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

set_target_properties(xlspp-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(xlspp-obj PUBLIC include)

target_link_libraries(xlspp-obj
    PUBLIC
        tl::expected
    PRIVATE
        fmt::fmt
        libzip::zip
        Microsoft.GSL::GSL
        tinyxml2::tinyxml2
)


add_library(xlspp SHARED)

target_link_libraries(xlspp PUBLIC xlspp-obj)


add_library(xlspp-static STATIC)

target_include_directories(xlspp-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(xlspp-static PUBLIC xlspp-obj)
