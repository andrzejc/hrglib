include(FindPackageHandleStandardArgs)
find_program(COG_SCRIPT NAMES cog.py cog)
find_package_handle_standard_args(Cog DEFAULT_MSG COG_SCRIPT)

if(Cog_FOUND AND NOT TARGET Cog)
    add_executable(Cog IMPORTED GLOBAL)
    set_target_properties(Cog PROPERTIES
        IMPORTED_LOCATION "${COG_SCRIPT}"
    )
endif()
