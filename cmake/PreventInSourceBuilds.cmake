if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Instead, build out-of-source, e.g.:\n"
        "mkdir build\n"
        "cmake -B build -S ."
        "\n"
        "In order to remove already created in-source build remnants, execute:\n"
        "rm -rf CMakeFiles CMakeCache.txt\n"
    )
endif()
