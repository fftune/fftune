project(fftune)
cmake_minimum_required(VERSION 3.23)

option(BUILD_TESTING "Build the testing tree." OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("src")

file(GLOB_RECURSE SRCS "src/*.cpp")
file(GLOB_RECURSE HDRS "src/*.hpp")

# dependencies
find_package(PkgConfig REQUIRED)
list(APPEND PKGCONFIG_MODULES "sfizz" "fftw3f" "smf" "sndfile")
foreach(PKG IN LISTS PKGCONFIG_MODULES)
	pkg_check_modules("${PKG}" REQUIRED IMPORTED_TARGET "${PKG}")
endforeach()
list(TRANSFORM PKGCONFIG_MODULES PREPEND "PkgConfig::")

add_library("${PROJECT_NAME}" SHARED ${SRCS})
target_link_libraries("${PROJECT_NAME}" ${PKGCONFIG_MODULES})
target_sources("${PROJECT_NAME}" PUBLIC FILE_SET HEADERS BASE_DIRS "src" FILES ${HDRS})

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}" FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/man/" TYPE MAN)


# tools
add_subdirectory(tools)

# tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	include(CTest)
	add_subdirectory(tests)
endif()
