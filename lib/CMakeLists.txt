set(CMAKE_AUTOMOC ON)

set(sources
    "include/qnodes/bezier.hpp"
    "include/qnodes/connection.hpp"
    "include/qnodes/node.hpp"
    "include/qnodes/slot.hpp"
    
    "src/bezier.cpp"
    "src/connection.cpp"
    "src/node.cpp"
    "src/slot.cpp"
)

add_library(qnodes STATIC ${sources})
target_include_directories(qnodes PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(qnodes PUBLIC Qt5::Widgets)
