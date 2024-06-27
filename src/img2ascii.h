#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define IMG2ASCII_EXPORT __declspec(dllexport)
#else
  #define IMG2ASCII_EXPORT
#endif

IMG2ASCII_EXPORT void img2ascii();
IMG2ASCII_EXPORT void img2ascii_print_vector(const std::vector<std::string> &strings);
