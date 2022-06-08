# Testing Suite

This directory contains a testing suite based on [GoogleTest](https://github.com/google/googletest) to make it easy to find regressions.

Tests are not built by default, so you will have to explicitly enable them:
```bash
cmake -B build -DBUILD_TESTING=ON
cmake --build build
# Run the tests
cmake --build build --target test
```
