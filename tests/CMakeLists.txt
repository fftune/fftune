set(TESTING_TARGET tests)
file(GLOB_RECURSE TESTING_SRCS "*.cpp")

include(GoogleTest)
pkg_check_modules(gtest REQUIRED IMPORTED_TARGET gtest)

add_executable("${TESTING_TARGET}" ${TESTING_SRCS})
target_link_libraries("${TESTING_TARGET}" "${PROJECT_NAME}" PkgConfig::gtest gtest_main)
gtest_discover_tests("${TESTING_TARGET}")
