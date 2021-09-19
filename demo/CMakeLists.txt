set(sources
    "src/demo_nodes.cpp"
    "src/demo_nodes.hpp"
    "src/main_window.cpp"
    "src/main_window.hpp"
    "src/main.cpp"
)

set(resources
    "QDarkStyleSheet/qdarkstyle/dark/style.qrc"
)

qt5_add_resources(rcc_generated_sources ${resources})

add_executable(qnodes_demo WIN32 ${sources} ${rcc_generated_sources})
target_link_libraries(qnodes_demo PRIVATE qnodes)
