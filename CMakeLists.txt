cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_USER_MAKE_RULES_OVERRIDE cmake/MakeRulesOverride.cmake)

project(HRGLIB
    VERSION 0.0.1
    DESCRIPTION "Heterogenous Relation Graph Object Model Library"
    LANGUAGES CXX
)

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
    set(HRGLIB_IS_TOPLEVEL TRUE)
endif()
option(HRGLIB_BUILD_TESTING "Build tests?" "${HRGLIB_IS_TOPLEVEL}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(GNUInstallDirs)
include(IWYU)
include(PythonVirtualEnv)

# We use python cog package as build tool
python_virtualenv_setup(VERSION 3.6)

include(FindPackageOrEnableInternalDep)

find_package_or_enable_internal_dep(HRGLIB_BUILD_INTERNAL_yaml-cpp yaml-cpp)
if(HRGLIB_BUILD_TESTING)
    find_package_or_enable_internal_dep(HRGLIB_BUILD_INTERNAL_GTest GTest)
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(lang IN ITEMS C CXX)
        foreach(cfg IN ITEMS "" "_${CMAKE_BUILD_TYPE}")
            string(TOUPPER "${cfg}" cfg)
            if(CMAKE_${lang}_FLAGS${cfg})
                string(REGEX REPLACE "(^| )-g([0-9]*)( |$)" "\\1-ggdb3\\3"
                        "CMAKE_${lang}_FLAGS${cfg}" "${CMAKE_${lang}_FLAGS${cfg}}")
            endif()
        endforeach()
    endforeach()
    add_compile_options(-fdiagnostics-color=always)
endif()

add_subdirectory(deps)

if(MSVC OR CMAKE_CXX_SIMULATE_ID MATCHES MSVC)
    add_compile_definitions(
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        _CRT_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
    )
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(docs)

if(HRGLIB_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif(HRGLIB_BUILD_TESTING)

set(export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
install(
    EXPORT      "${PROJECT_NAME}Targets"
    NAMESPACE   "${PROJECT_NAME}::"
    DESTINATION "${export_dir}"
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    cmake/Config.cmake.in
    "${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${export_dir}"
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION "${export_dir}"
)
