#include "utils.h"
#include <iostream>
#include <limits>

using namespace std;

// Функция для очистки ввода
void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для безопасного ввода целого числа с проверкой диапазона
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
                clearInput(); // Очищаем остаток строки
                return value;
            }
            else
            {
                cout << "Значение должно быть между " << minVal << " и " << maxVal << ". Попробуйте снова.\n";
            }
        }
        else
        {
            cout << "Ошибка ввода. Введите число.\n";
        }
        clearInput();
    }
}
