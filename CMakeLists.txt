cmake_minimum_required(VERSION 3.15)
project(img2ascii CXX)



add_executable(img2ascii src/img2ascii.cpp src/main.cpp)



install(TARGETS img2ascii DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
