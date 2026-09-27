#include <iostream>
#include <cmath>
#include <cstdlib>
#include <random>

using namespace std;

class IncorrectNumber {};

// Переводит целое неотрицательное число в код Грея
int conversionToGrayCode(int i) {
    try {
        if (i < 0) {
            throw IncorrectNumber();
        }
        else {
            return (i ^ (i >> 1));
        }
    }
    catch (IncorrectNumber) {
        cout << "Number less than zero: " << i << endl;
        exit(1);
    }
}

// Выводит универсум
void universeOutput(int uni[], int size) {
    cout << "Universe: \n";

    if (size == 1) {
        cout << "1. " << uni[0];
        cout << endl;
    }
    else if (size == 2) {
        cout << "1. " << uni[0] << "        ";
        cout << "2. " << uni[1];
        cout << endl;
    }
    else {
        int count = 0;
        for (int i = 0; i < size / 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << count << ". " << uni[i * 4 + j] << "        ";
                count += 1;
            }
            cout << endl;
        } 
    }
}

// Заполняет универсум
bool fillTheUniverse(int uni[], int size) {
    for (int i = 0; i < size; i++) {
        uni[i] = conversionToGrayCode(i);
    }

    universeOutput(uni, size);
    return true;
}

// Выводит мультимножество
void multisetOutput(int uni[][2], int elementCount) {
    cout << "\nMultiset elements:\n";
    if (elementCount == 0) {
        cout << "Multiset is empty.\n";
        return;
    }
    for (int i = 0; i < elementCount; i++) {
        cout << i << ". Gray code: " << uni[i][0] << "; Multiplicity: " << uni[i][1] << endl;
    }
}

// Ручное заполнение мультимножества
int manualMethodOfFillingMultiset(int uni[][2], int power, int size) {
    int i = 0;
    int remainingPower = power;

    while (remainingPower > 0) {
        int number = 0;
        cout << "\nEnter the number you want\n to convert to gray code and write to the multiset: ";
        cin >> number;

        while (number < 0 || number >= size) {
            cout << "\nThe number must be between 0 inclusive and " << size << " not inclusive!" << endl;
            cout << "Try again: ";
            cin >> number;
        }

        int count = 0;
        cout << "Enter its multiplicity (1 to " << remainingPower << "): ";
        cin >> count;

        while (count < 1 || count > remainingPower) {
            cout << "\nThe multiplicity must be between 1 and " << remainingPower << endl;
            cout << "Try again: ";
            cin >> count;
        }

        uni[i][0] = conversionToGrayCode(number);
        uni[i][1] = count;

        remainingPower -= count;
        i += 1;
    }

    multisetOutput(uni, i);
    return i;
}

// Автоматическое заполнение мультимножества
int automaticMethodOfFillingMultiset(int uni[][2], int power, int size) {
    random_device rd;
    mt19937 gen(rd());

    int i = 0;
    int remainingPower = power;

    while (remainingPower > 0) {
        uniform_int_distribution<int> distNumber(0, size - 1);
        int number = distNumber(gen);

        uniform_int_distribution<int> distCount(1, remainingPower);
        int count = distCount(gen);

        uni[i][0] = conversionToGrayCode(number);
        uni[i][1] = count;

        remainingPower -= count;
        i += 1;
    }

    multisetOutput(uni, i);
    return i;
}


// Реализация всех функций


// Возвращает кратность элемента code в мультимножестве
// (если элемент записан несколько раз, кратности суммируются)
int getMultiplicity(int ms[][2], int elementCount, int code) {
    int multiplicity = 0;
    for (int i = 0; i < elementCount; i++) {
        if (ms[i][0] == code) {
            multiplicity += ms[i][1];
        }
    }
    return multiplicity;
}

// Добавляет элемент в результат, если его кратность больше нуля
void addElement(int res[][2], int &resCount, int code, int multiplicity) {
    if (multiplicity > 0) {
        res[resCount][0] = code;
        res[resCount][1] = multiplicity;
        resCount += 1;
    }
}


// ---------- Теоретико-множественные операции ----------

// Объединение: max(a, b)
int multisetUnion(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], max(ka, kb));
    }
    return resCount;
}

// Пересечение: min(a, b)
int multisetIntersection(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], min(ka, kb));
    }
    return resCount;
}

// Дополнение: U - a
int multisetComplement(int a[][2], int aCount, int uni[], int size, int universeMultiplicity, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        addElement(res, resCount, uni[i], universeMultiplicity - ka);
    }
    return resCount;
}

// Разность A \ B = A ∩ not(B): min(a, U - b)
int multisetDifference(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int universeMultiplicity, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], min(ka, universeMultiplicity - kb));
    }
    return resCount;
}

// Симметрическая разность (A \ B) ∪ (B \ A): max(min(a, U - b), min(b, U - a))
int multisetSymmetricDifference(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int universeMultiplicity, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        int aWithoutB = min(ka, universeMultiplicity - kb);
        int bWithoutA = min(kb, universeMultiplicity - ka);
        addElement(res, resCount, uni[i], max(aWithoutB, bWithoutA));
    }
    return resCount;
}


// ---------- Арифметические операции ----------

// Арифметическая сумма: a + b
int multisetArithmeticSum(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], ka + kb);
    }
    return resCount;
}

// Арифметическая разность: a - b, если a > b, иначе 0
int multisetArithmeticDifference(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], max(ka - kb, 0));
    }
    return resCount;
}

// Арифметическое произведение: a * b
int multisetArithmeticProduct(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        addElement(res, resCount, uni[i], ka * kb);
    }
    return resCount;
}

// Арифметическое деление: a / b (целая часть), если b > 0, иначе 0
int multisetArithmeticDivision(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int res[][2]) {
    int resCount = 0;
    for (int i = 0; i < size; i++) {
        int ka = getMultiplicity(a, aCount, uni[i]);
        int kb = getMultiplicity(b, bCount, uni[i]);
        if (kb > 0) {
            addElement(res, resCount, uni[i], ka / kb);
        }
    }
    return resCount;
}


// Вызывает все операции и выводит их результаты
void allOperationsOutput(int a[][2], int aCount, int b[][2], int bCount, int uni[], int size, int universeMultiplicity) {
    int res[size][2];
    int resCount = 0;

    cout << "\n\n\nSet-theoretic operations\n";
    cout << "(Universe multiplicity of each element: " << universeMultiplicity << ")\n";

    cout << "\nUnion A U B";
    resCount = multisetUnion(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);

    cout << "\nIntersection A ^ B";
    resCount = multisetIntersection(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);

    cout << "\nDifference A \\ B";
    resCount = multisetDifference(a, aCount, b, bCount, uni, size, universeMultiplicity, res);
    multisetOutput(res, resCount);

    cout << "\nDifference B \\ A";
    resCount = multisetDifference(b, bCount, a, aCount, uni, size, universeMultiplicity, res);
    multisetOutput(res, resCount);

    cout << "\nSymmetric difference A (+) B";
    resCount = multisetSymmetricDifference(a, aCount, b, bCount, uni, size, universeMultiplicity, res);
    multisetOutput(res, resCount);

    cout << "\nComplement not(A)";
    resCount = multisetComplement(a, aCount, uni, size, universeMultiplicity, res);
    multisetOutput(res, resCount);

    cout << "\nComplement not(B)";
    resCount = multisetComplement(b, bCount, uni, size, universeMultiplicity, res);
    multisetOutput(res, resCount);

    cout << "\n\n\nArithmetic operations\n";

    cout << "\nArithmetic sum A + B";
    resCount = multisetArithmeticSum(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);

    cout << "\nArithmetic difference A - B";
    resCount = multisetArithmeticDifference(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);

    cout << "\nArithmetic product A * B";
    resCount = multisetArithmeticProduct(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);

    cout << "\nArithmetic division A / B";
    resCount = multisetArithmeticDivision(a, aCount, b, bCount, uni, size, res);
    multisetOutput(res, resCount);
}
