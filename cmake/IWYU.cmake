if(_IWYUModuleIncluded)
    return()
else()
    set(_IWYUModuleIncluded TRUE)
endif()

option(ENABLE_IWYU "Enable Include-what-you use?" OFF)
if(ENABLE_IWYU)
    find_package(IWYU REQUIRED)
    set(IWYU_COMMAND "${IWYU_EXECUTABLE}" -Xiwyu "--mapping_file=${CMAKE_SOURCE_DIR}/etc/mappings.imp")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_COMMAND}")
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${IWYU_COMMAND}")
endif()