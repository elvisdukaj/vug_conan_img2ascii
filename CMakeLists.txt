cmake_minimum_required(VERSION 3.18)
project(usergroupt CXX)

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(img2ascii src/main.cpp)

target_link_libraries(img2ascii
  PUBLIC
    cxxopts::cxxopts
    fmt::fmt
    cpr::cpr
)
