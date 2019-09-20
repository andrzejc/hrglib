find_package(Doxygen)
option(HRGLIB_BUILD_DOCS "Build API docs?" "${DOXYGEN_FOUND}")
if(HRGLIB_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXCLUDE_PATTERNS
        "*.in.*"
        "${CMAKE_BINARY_DIR}/include/python*"
        )
    set(DOXYGEN_STRIP_FROM_INC_PATH
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
        )
    set(DOXYGEN_STRIP_FROM_PATH
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
        )
    doxygen_add_docs(api-docs
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
        )
endif()
