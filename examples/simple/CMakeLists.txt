cmake_minimum_required(VERSION 3.9)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS ../..)
project(pololu_simple_example)

