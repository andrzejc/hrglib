
if(BUILD_TESTING)
    enable_testing()
    option(TESTS_SEPARATE_EXECUTABLES "Build testsuites as separate executables?" OFF)

    set(TESTS
        test_feature_name
        test_features
        test_graph
        test_node
    )

    set(TEST_SRCS)
    set(TEST_DEPS HrgLib GTest::GTest GTest::Main)
    set(TEST_DEFS HRGLIB_YAMLCPP_PUBLIC)

    function(add_test_target name sources)
        add_executable("${name}" "${sources}")
        target_link_libraries("${name}" ${TEST_DEPS})
        target_compile_definitions("${name}" PRIVATE ${TEST_DEFS})
        add_test(NAME "${name}" COMMAND "${name}")
        set_property(TEST "${name}" APPEND PROPERTY
            ENVIRONMENT
                "HRGLIB_TEST_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
                "GTEST_COLOR=yes"
            )
    endfunction(add_test_target)

    foreach(test IN LISTS TESTS)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test}.cpp")
            message(FATAL_ERROR "Test file ${test}.cpp not found")
        endif()
        if(TESTS_SEPARATE_EXECUTABLES)
            add_test_target("${test}" "${test}.cpp")
        else()
            list(APPEND TEST_SRCS "${test}.cpp")
        endif()
    endforeach(test)
    if(NOT TESTS_SEPARATE_EXECUTABLES)
        add_test_target(unit_tests "${TEST_SRCS}")
    endif()
endif(BUILD_TESTING)
