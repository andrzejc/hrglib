if(_FindPackageOrEnableInternalDepModuleIncluded)
    return()
else()
    set(_FindPackageOrEnableInternalDepModuleIncluded TRUE)
endif()

macro(find_package_or_enable_internal_dep enable_dep_var package)
    if(NOT DEFINED "${enable_dep_var}")
        find_package("${package}" ${ARGN} QUIET)
        if(${package}_FOUND)
            find_package("${package}" ${ARGN} REQUIRED)
            option("${enable_dep_var}" "" OFF)
        else()
            option("${enable_dep_var}" "" ON)
        endif()
    endif()
endmacro(find_package_or_enable_internal_dep)
