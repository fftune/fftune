cmake_minimum_required(VERSION 3.23)
project(pipewire-realtime)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRCS "src/*.cpp")

find_package(PkgConfig REQUIRED)
list(APPEND PKGCONFIG_MODULES "fftune" "libpipewire-0.3")
foreach(PKG IN LISTS PKGCONFIG_MODULES)
	pkg_check_modules("${PKG}" REQUIRED IMPORTED_TARGET "${PKG}")
endforeach()
list(TRANSFORM PKGCONFIG_MODULES PREPEND "PkgConfig::")

add_executable("${PROJECT_NAME}" ${SRCS})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${PKGCONFIG_MODULES})

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}")
