add_custom_target(format-dry-run
    COMMAND python3 scripts/runformat.py --dry-run --sources examples src tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format (dry run)"
)

add_custom_target(format
    COMMAND python3 scripts/runformat.py --sources examples src tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format"
)
