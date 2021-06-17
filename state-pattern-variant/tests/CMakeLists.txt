set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_tests)
project(PROJECT_TEST_NAME)

file(GLOB_RECURSE TEST_SRC_FILES ./*_tests.cpp)

add_executable (${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_LIB_NAME})

enable_testing(true)

add_test(tests ${PROJECT_TEST_NAME})