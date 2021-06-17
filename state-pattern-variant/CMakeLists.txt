get_filename_component(PROJECT_NAME_STR ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME_STR})

cmake_minimum_required(VERSION 2.8)
project(${PROJECT_NAME_STR})

#----------------------------------------
# set Threads
#----------------------------------------
find_package(Threads REQUIRED)


#----------------------------------------
# Library - src
#----------------------------------------
add_subdirectory(src)

#----------------------------------------
# Application
#----------------------------------------
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} turnstile_lib)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

#----------------------------------------
# Tests
#----------------------------------------
enable_testing()
add_subdirectory(tests)