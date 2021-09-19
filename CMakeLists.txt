cmake_minimum_required(VERSION 3.12)

project(qnodes)

option(QNodes_ENABLE_DEMO "Build demo app?" ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(lib)

if(QNodes_ENABLE_DEMO)
    add_subdirectory(demo)
endif()
