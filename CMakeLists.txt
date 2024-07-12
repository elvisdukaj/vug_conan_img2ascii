cmake_minimum_required(VERSION 3.18)
project(usergroupt CXX)

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

add_executable(img2ascii src/main.cpp)

target_compile_options(img2ascii
PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_link_libraries(img2ascii
PRIVATE
    cxxopts::cxxopts
    fmt::fmt
    cpr::cpr
    opencv::opencv_imgcodecs_alias
    opencv::opencv_imgproc_alias
)
