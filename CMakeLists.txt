cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Coverage)

project(HrgLib
    VERSION 0.0.1
    DESCRIPTION "Heterogenous Relation Graph Object Model Library"
)

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
    set(HRGLIB_IS_TOPLEVEL TRUE)
endif()
option(BUILD_TESTING "Build tests?" "${HRGLIB_IS_TOPLEVEL}")
option(BUILD_SHARED_LIBS "Build shared libs?" "${BUILD_SHARED_LIBS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(GNUInstallDirs)
include(IWYU)
include(PythonVirtualEnv)

# We use python cog package as build tool
python_virtualenv_setup(VERSION 3.6)

option(BUILD_INTERNAL_DEPS "Build deps as internal subprojects?" OFF)
if(BUILD_INTERNAL_DEPS)
    add_subdirectory(deps/gsl-lite)
    add_subdirectory(deps/yaml-cpp)
    add_subdirectory(deps/googletest)
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
else()
    # Public
    find_package(gsl-lite REQUIRED)

    # Private
    find_package(yaml-cpp REQUIRED)
    find_package(GTest REQUIRED)
endif()

find_package(Boost REQUIRED)

if(MSVC OR CMAKE_CXX_SIMULATE_ID MATCHES MSVC)
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
