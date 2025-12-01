#include "utils.h"
#include <iostream>
#include <limits>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

using namespace std;

// Function to clear input
void clearInput()
{
    cin.clear();
    cin.ignore(1000, '\n');
}

// Function for safe integer input with range check
int getSafeIntInput(const string &prompt, int minVal, int maxVal)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            if (value >= minVal && value <= maxVal)
            {
                clearInput(); // Clear the rest of the line
                return value;
            }
            else
            {
                cout << "Value must be between " << minVal << " and " << maxVal << ". Try again.\n";
            }
        }
        else
        {
            cout << "Input error. Enter a number.\n";
        }
        clearInput();
    }
}

// Function to convert CP1251 string to UTF-8
string cp1251_to_utf8(const string &str)
{
    if (str.empty()) return str;

    // Convert CP1251 to wide string
    int size_needed = MultiByteToWideChar(1251, 0, str.c_str(), -1, NULL, 0);
    wstring wstr(size_needed, 0);
    MultiByteToWideChar(1251, 0, str.c_str(), -1, &wstr[0], size_needed);

    // Convert wide string to UTF-8
    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    string utf8_str(utf8_size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8_str[0], utf8_size, NULL, NULL);

    // Remove null terminator if present
    if (!utf8_str.empty() && utf8_str.back() == '\0') {
        utf8_str.pop_back();
    }

    return utf8_str;
}
