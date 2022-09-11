cmake_minimum_required(VERSION 2.6)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests Test_Point.cpp Test_Node.cpp Test_Utils.cpp Test_Game.cpp Test_MiniMax.cpp Test_Heuristic.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
