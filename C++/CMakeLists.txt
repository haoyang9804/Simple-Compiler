cmake_minimum_required(VERSION 3.2)

project(interp C CXX)

add_library(${PROJECT_NAME} SHARED src/interp.cpp)

include(CTest)

add_executable(testExpr test/testExpr.cpp)
target_link_libraries(testExpr ${PROJECT_NAME})
add_test(NAME testExpr COMMAND testExpr)

