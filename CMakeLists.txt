cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Coverage)

project(HrgLib
    VERSION 0.0.0
    DESCRIPTION "Heterogenous Relation Graph Object Model Library"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(IWYU)
include(CTest)
include(PythonVirtualEnv)

# We use python cog package as build tool
python_virtualenv_setup(VERSION 3.6)

# Public
find_package(gsl-lite REQUIRED)
find_package(Boost REQUIRED)

# Private
find_package(yaml-cpp REQUIRED)

set(pkg_name    "${PROJECT_NAME}")
set(pkg_version "${${PROJECT_NAME}_VERSION}")
set(pkg_target  "${pkg_name}Targets")

message(STATUS "Project '${PROJECT_NAME}', package '${pkg_name}' version: '${pkg_version}'")

if(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL MSVC)
    add_compile_definitions(
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        _CRT_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
    )
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(docs)

set(cmake_files_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${pkg_name}")

set(version_file "${CMAKE_CURRENT_BINARY_DIR}/${pkg_name}ConfigVersion.cmake")
write_basic_package_version_file(
    "${version_file}"
    COMPATIBILITY AnyNewerVersion
)

set(config_file "${CMAKE_CURRENT_BINARY_DIR}/${pkg_name}Config.cmake")
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${config_file}"
    INSTALL_DESTINATION "${cmake_files_install_dir}"
)

install(
    EXPORT "${pkg_target}"
    NAMESPACE "${pkg_name}::"
    DESTINATION "${cmake_files_install_dir}"
)
install(
    FILES
        "${version_file}"
        "${config_file}"
    DESTINATION "${cmake_files_install_dir}"
)
