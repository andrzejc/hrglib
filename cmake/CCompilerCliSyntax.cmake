if(_CCompilerCliSyntaxModuleIncluded)
    return()
else()
    set(_CCompilerCliSyntaxModuleIncluded TRUE)
endif()

set(CMAKE_C_COMPILER_CLI_SYNTAX_GNU OFF)

if((CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" OR
    CMAKE_C_SIMULATE_ID STREQUAL "GNU" OR CMAKE_CXX_SIMULATE_ID STREQUAL "GNU") AND
    NOT (CMAKE_C_SIMULATE_ID STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
    set(CMAKE_C_COMPILER_CLI_SYNTAX_GNU ON)
endif()
