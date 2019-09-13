if(_PythonVirtualEnvModuleIncluded)
    return()
else()
    set(_PythonVirtualEnvModuleIncluded TRUE)
endif()

function(python_virtualenv_setup)
    cmake_parse_arguments(arg NO_CREATE "DIR;VERSION" ""  ${ARGN})
    if(NOT IS_ABSOLUTE "${arg_DIR}")
        set(arg_DIR "${CMAKE_CURRENT_BINARY_DIR}/${arg_DIR}")
    endif()
    if(NOT EXISTS "${arg_DIR}/bin/python" AND NOT arg_NO_CREATE)
        message(STATUS "Installing python virtualenv in ${arg_DIR}...")
        find_package(Python ${arg_VERSION} REQUIRED COMPONENTS Interpreter)
        execute_process(
            COMMAND "${Python_EXECUTABLE}" -m virtualenv "${arg_DIR}"
            RESULT_VARIABLE res
            )
        if(NOT res EQUAL 0)
            message(FATAL_ERROR "virtualenv failed (${res})")
        endif()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
            message(STATUS "Installing python requirements in virtualenv...")
            execute_process(
                COMMAND "${arg_DIR}/bin/python" -m pip
                    install  --no-warn-script-location
                        -r "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
                RESULT_VARIABLE res
                )
            if(NOT res EQUAL 0)
                message(FATAL_ERROR "pip failed (${res})")
            endif()
        endif()
    endif()
    if(EXISTS "${arg_DIR}/bin/python")
        message(STATUS "Adding ${arg_DIR}/bin to CMAKE_PROGRAM_PATH")
        list(APPEND CMAKE_PROGRAM_PATH "${arg_DIR}/bin")
        set(CMAKE_PROGRAM_PATH "${CMAKE_PROGRAM_PATH}" PARENT_SCOPE)
        unset(Python_EXECUTABLE CACHE)
        find_package(Python ${arg_VERSION} REQUIRED)
    endif()
endfunction(python_virtualenv_setup)
