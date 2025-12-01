#pragma once
#include <string>
#include <limits>
#include <iostream>

// Utility functions
void clearInput();
int getSafeIntInput(const std::string &prompt, int minVal, int maxVal);
std::string cp1251_to_utf8(const std::string &str);
