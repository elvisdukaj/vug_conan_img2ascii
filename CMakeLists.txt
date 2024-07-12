cmake_minimum_required(VERSION 3.18)
project(usergroupt CXX)

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

add_executable(img2ascii src/main.cpp)

target_compile_features(img2ascii PRIVATE cxx_std_23)

target_link_libraries(img2ascii
  PRIVATE
    cxxopts::cxxopts
    fmt::fmt
    cpr::cpr
    opencv::opencv_imgcodecs_alias
    opencv::opencv_imgproc_alias
)
