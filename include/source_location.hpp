#pragma once
#include <string>

#include <pl/current_function.hpp>
#include <pl/source_line.hpp>

#define VC_SOURCE_LOCATION                                                     \
  "file: " __FILE__ "\nline: " PL_SOURCE_LINE "\nfunction:"                    \
  " "                                                                          \
    + std::string(PL_CURRENT_FUNCTION)
