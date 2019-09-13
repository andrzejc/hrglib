set(SRCS
    error.cpp
    feature_name.cpp
    features.cpp
    graph.cpp
    node.cpp
    relation.cpp
    relation_name.cpp
)

add_library(HrgLib ${SRCS})
add_dependencies(HrgLib generate-headers)
set_target_properties(HrgLib PROPERTIES
    OUTPUT_NAME hrg)
target_link_libraries(HrgLib
    PUBLIC
        Boost::boost
        gsl::gsl-lite
    PRIVATE
        "${YAML_CPP_LIBRARIES}"
)
target_include_directories(HrgLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        "${YAML_CPP_INCLUDE_DIRS}"
)
install(TARGETS HrgLib EXPORT HrgLib
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(EXPORT HrgLib
    NAMESPACE HrgLib::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/HrgLib"
)
