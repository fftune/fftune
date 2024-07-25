find_package(PkgConfig REQUIRED)
list(APPEND PKGCONFIG_MODULES "fftw3f" "smf" "sndfile")
foreach(PKG IN LISTS PKGCONFIG_MODULES)
	pkg_check_modules("${PKG}" REQUIRED IMPORTED_TARGET "${PKG}")
endforeach()

include("${CMAKE_CURRENT_LIST_DIR}/fftune-targets.cmake")
