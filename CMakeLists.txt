cmake_minimum_required(VERSION 3.18)
project(usergroupt CXX)

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)

add_executable(img2ascii src/main.cpp)

target_compile_definitions(img2ascii PRIVATE STB_IMAGE_IMPLEMENTATION STB_IMAGE_RESIZE2_IMPLEMENTATION)

target_link_libraries(img2ascii
  PRIVATE
    cxxopts::cxxopts
    fmt::fmt
    cpr::cpr
    stb::stb
)
