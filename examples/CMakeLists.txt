find_package(fmt CONFIG REQUIRED)

add_executable(simple_write)

target_sources(simple_write PRIVATE simple_write.cpp)

target_link_libraries(simple_write PRIVATE xlspp fmt::fmt)


add_executable(simple_write_extended)

target_sources(simple_write_extended PRIVATE simple_write_extended.cpp)

target_link_libraries(simple_write_extended PRIVATE xlspp fmt::fmt)
