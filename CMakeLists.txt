cmake_minimum_required(VERSION 3.21)
project(squarereq)

set(CMAKE_CXX_STANDARD 23)

add_executable(squarereq main.cpp)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(rxcpp CONFIG)

include_directories("~/Documents/sc/c++/zeromq/matching.engine" "~/Documents/sc/c++/zeromq/matching_engine_test_utils")

target_link_libraries(squarereq PRIVATE debug nlohmann_json::nlohmann_json cppzmq  ~/lib/matching_engine/debug/libmatching_engine.so ~/lib/matching_engine/test_utils/debug/libmatching_engine_test_utils.so)

#target_link_libraries(squarereq PRIVATE optimized nlohmann_json::nlohmann_json cppzmq ~/lib/matching_engine/release/libmatching_engine.so ~/lib/matching_engine/test_utils/release/libmatching_engine_test_utils.so)
