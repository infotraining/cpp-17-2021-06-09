set(PROJECT_LIB_NAME turnstile_lib)
set(PROJECT_LIB_NAME ${PROJECT_LIB_NAME} turnstile_lib PARENT_SCOPE)
project(Turnstile_lib)

add_library(${PROJECT_LIB_NAME} STATIC turnstile.cpp turnstile.hpp)
target_include_directories(${PROJECT_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${PROJECT_LIB_NAME} PUBLIC cxx_std_17)