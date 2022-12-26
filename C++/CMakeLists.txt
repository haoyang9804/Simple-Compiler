cmake_minimum_required(VERSION 3.2)

project(interp C CXX)

add_library(${PROJECT_NAME} SHARED src/compiler.cpp)

include(CTest)

add_executable(testExpr test/test.cpp)
target_link_libraries(testExpr ${PROJECT_NAME})
add_test(NAME testExpr COMMAND testExpr)

