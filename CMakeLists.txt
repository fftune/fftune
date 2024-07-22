cmake_minimum_required(VERSION 3.23)
cmake_policy(SET CMP0048 NEW)
project(fftune VERSION 1.0 DESCRIPTION "Pitch detection library")

option(BUILD_TESTING "Build the testing tree." OFF)
option(USE_SFIZZ "Build with sfizz support." OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("src")
add_compile_definitions(FFTUNE_VERSION="${PROJECT_VERSION}")

file(GLOB_RECURSE SRCS "src/*.cpp")
file(GLOB_RECURSE HDRS "src/*.hpp")

# dependencies
find_package(PkgConfig REQUIRED)
list(APPEND PKGCONFIG_MODULES "fftw3f" "smf" "sndfile")

if(USE_SFIZZ)
	list(APPEND PKGCONFIG_MODULES "sfizz")
	add_compile_definitions(HAS_SFIZZ)
endif()

foreach(PKG IN LISTS PKGCONFIG_MODULES)
	pkg_check_modules("${PKG}" REQUIRED IMPORTED_TARGET "${PKG}")
endforeach()
string(REPLACE ";" " " PKGCONFIG_REQUIRES "${PKGCONFIG_MODULES}")
list(TRANSFORM PKGCONFIG_MODULES PREPEND "PkgConfig::")


add_library("${PROJECT_NAME}" SHARED ${SRCS})
set_target_properties("${PROJECT_NAME}" PROPERTIES VERSION "${PROJECT_VERSION}")
target_link_libraries("${PROJECT_NAME}" ${PKGCONFIG_MODULES})
target_sources("${PROJECT_NAME}" PUBLIC FILE_SET HEADERS BASE_DIRS "src" FILES ${HDRS})

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}" FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/man/" TYPE MAN)
configure_file("${CMAKE_SOURCE_DIR}/assets/fftune.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/fftune.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fftune.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")


# tools
add_subdirectory(tools)

# tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	include(CTest)
	add_subdirectory(tests)
endif()
