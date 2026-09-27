#include <iostream>
#include "gray_code.cpp"

using namespace std;

int main()
{
    int n, size;
    cout << "Enter bit depth n: ";
    cin >> n;

    while (n < 0 || n > 30) {
        cout << "Bit depth must be between 0 and 30. Try again: ";
        cin >> n;
    }

    size = 1 << n;
    int universe[size] = {};

    fillTheUniverse(universe, size);

    // Формат записи: [[GrayCode, multiplicity], ...]

    // Первое мультимножество
    int choice1;

    do {
        cout << "\n\n\nSelect the method of forming the first multiset (enter number):\n 1. Manually 2. Automatic" << endl;
        cin >> choice1;
    }
    while (choice1 != 1 && choice1 != 2);

    int power1;
    do {
        cout << "\n\n\nEnter the power of the set (non-negative integer): " << endl;
        cin >> power1;
    }
    while (power1 < 0);

    int multiset1[power1 == 0 ? 1 : power1][2] = {};
    int count1 = 0;

    if (choice1 == 1) {
        count1 = manualMethodOfFillingMultiset(multiset1, power1, size);
    }
    else {
        count1 = automaticMethodOfFillingMultiset(multiset1, power1, size);
    }

    // Второе мультимножество
    int choice2;

    do {
        cout << "\n\n\nSelect the method of forming the second multiset (enter number):\n 1. Manually 2. Automatic" << endl;
        cin >> choice2;
    }
    while (choice2 != 1 && choice2 != 2);

    int power2;
    do {
        cout << "\n\n\nEnter the power of the set (non-negative integer): " << endl;
        cin >> power2;
    }
    while (power2 < 0);

    int multiset2[power2 == 0 ? 1 : power2][2] = {};
    int count2 = 0;

    if (choice2 == 1) {
        count2 = manualMethodOfFillingMultiset(multiset2, power2, size);
    }
    else {
        count2 = automaticMethodOfFillingMultiset(multiset2, power2, size);
    }

    // Кратность каждого элемента в универсуме: ни одна кратность
    // в A или B не может превысить мощность своего мультимножества
    int universeMultiplicity = max(power1, power2);

    allOperationsOutput(multiset1, count1, multiset2, count2, universe, size, universeMultiplicity);

    return 0;
}

