find_package(Cog REQUIRED)

add_custom_command(
    OUTPUT hrglib/feature_list.hpp
    COMMAND Cog
            -d
            -o hrglib/feature_list.hpp
            -D "FEATURES_YAML=${CMAKE_CURRENT_SOURCE_DIR}/hrglib/features.yaml"
            "${CMAKE_CURRENT_SOURCE_DIR}/hrglib/feature_list.in.hpp"
    DEPENDS
        Cog
        hrglib/feature_list.in.hpp
        hrglib/features.yaml
    VERBATIM
)

add_custom_target(generate-headers DEPENDS hrglib/feature_list.hpp)

install(
    DIRECTORY
        hrglib
        "${CMAKE_CURRENT_BINARY_DIR}/hrglib"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING REGEX "\\.(h|hxx|hpp|yaml)$"
    PATTERN "*.in.*" EXCLUDE
)
