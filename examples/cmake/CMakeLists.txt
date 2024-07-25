cmake_minimum_required(VERSION 3.23)
project(cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRCS "src/*.cpp")

# depend on fftune
find_package(fftune REQUIRED)

add_executable("${PROJECT_NAME}" ${SRCS})
target_link_libraries("${PROJECT_NAME}" fftune::fftune)

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}")
