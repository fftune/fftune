project(basic)
cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRCS "src/*.cpp")

# depend on fftune
find_package(PkgConfig REQUIRED)
pkg_check_modules(fftune REQUIRED IMPORTED_TARGET fftune)

add_executable("${PROJECT_NAME}" ${SRCS})
target_link_libraries("${PROJECT_NAME}" PkgConfig::fftune)

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}")
