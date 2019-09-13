cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Coverage)

project(HrgLib
    VERSION 0.0.0
    DESCRIPTION "Heterogenous Relation Graph Object Model Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(IWYU)
include(CTest)
include(PythonVirtualEnv)

# We use python cog package as build tool
python_virtualenv_setup(VERSION 3.6)

find_package(gsl-lite REQUIRED)
find_package(Boost REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(docs)