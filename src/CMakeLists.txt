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
        gsl-lite::gsl-lite
    PRIVATE
        "${YAML_CPP_LIBRARIES}"
)
target_include_directories(HrgLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "${YAML_CPP_INCLUDE_DIRS}"
)
install(TARGETS HrgLib EXPORT HrgLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(EXPORT HrgLib
    NAMESPACE HrgLib::
    DESTINATION lib/cmake/HrgLib
)
