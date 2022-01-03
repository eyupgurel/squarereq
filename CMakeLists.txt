cmake_minimum_required(VERSION 3.20)
project(squarereq)

set(CMAKE_CXX_STANDARD 20)

add_executable(squarereq main.cpp)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(squarereq PRIVATE nlohmann_json::nlohmann_json cppzmq)