#pragma once
#include <string>
#include <limits>
#include <iostream>

// Utility functions
void clearInput();
int getSafeIntInput(const std::string &prompt, int minVal, int maxVal);
