cmake_minimum_required(VERSION 3.2)

# set (MACOSX FALSE)
# set (LINUX FALSE)

# if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#   set (MACOSX TRUE)
# elseif (${CMAKE_SYSTEM_NAME} MATCHES "linux")
#   set (LINUX TRUE)
# endif()

# message(${MACOSX})
# message(${LINUX})

# if (${MACOSX})
#   set (CMAKE_C_COMPILER /opt/homebrew/opt/llvm/bin/clang)
#   set (CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)
# endif()

set (CMAKE_CXX_STANDARD 17)

project(interp C CXX)

add_library(${PROJECT_NAME} SHARED src/compiler.cpp)

include(CTest)

add_executable(testExpr test/test.cpp)
target_link_libraries(testExpr ${PROJECT_NAME})
add_test(NAME testExpr COMMAND testExpr)

