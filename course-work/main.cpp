// Правило «+1»:  a->b, b->e, c->a, d->a, e->h, f->g, g->{c|d}, h->f
// Берём g+1 = c, элемент d равносилен c (на вводе d заменяется на c, на выводе печатаются обе записи).
// Цикл (порядок цифр): a < b < e < h < f < g < c.  Цифры позиционной записи: a b e h f g c.

#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

using Row = map<char, char>;
using Table = map<char, Row>;
using DivRow = map<char, pair<char, char>>;  // {частное, остаток}
using DivTab = map<char, DivRow>;

static const Table ADD = {
    {'a', {{'a','a'},{'b','b'},{'c','c'},{'e','e'},{'f','f'},{'g','g'},{'h','h'}}},
    {'b', {{'a','b'},{'b','e'},{'c','a'},{'e','h'},{'f','g'},{'g','c'},{'h','f'}}},
    {'c', {{'a','c'},{'b','a'},{'c','g'},{'e','b'},{'f','h'},{'g','f'},{'h','e'}}},
    {'e', {{'a','e'},{'b','h'},{'c','b'},{'e','f'},{'f','c'},{'g','a'},{'h','g'}}},
    {'f', {{'a','f'},{'b','g'},{'c','h'},{'e','c'},{'f','b'},{'g','e'},{'h','a'}}},
    {'g', {{'a','g'},{'b','c'},{'c','f'},{'e','a'},{'f','e'},{'g','h'},{'h','b'}}},
    {'h', {{'a','h'},{'b','f'},{'c','e'},{'e','g'},{'f','a'},{'g','b'},{'h','c'}}},
};

static const Table SUB = {
    {'a', {{'a','a'},{'b','c'},{'c','b'},{'e','g'},{'f','h'},{'g','e'},{'h','f'}}},
    {'b', {{'a','b'},{'b','a'},{'c','e'},{'e','c'},{'f','f'},{'g','h'},{'h','g'}}},
    {'c', {{'a','c'},{'b','g'},{'c','a'},{'e','f'},{'f','e'},{'g','b'},{'h','h'}}},
    {'e', {{'a','e'},{'b','b'},{'c','h'},{'e','a'},{'f','g'},{'g','f'},{'h','c'}}},
    {'f', {{'a','f'},{'b','h'},{'c','g'},{'e','e'},{'f','a'},{'g','c'},{'h','b'}}},
    {'g', {{'a','g'},{'b','f'},{'c','c'},{'e','h'},{'f','b'},{'g','a'},{'h','e'}}},
    {'h', {{'a','h'},{'b','e'},{'c','f'},{'e','b'},{'f','c'},{'g','g'},{'h','a'}}},
};

static const Table MUL = {
    {'a', {{'a','a'},{'b','a'},{'c','a'},{'e','a'},{'f','a'},{'g','a'},{'h','a'}}},
    {'b', {{'a','a'},{'b','b'},{'c','c'},{'e','e'},{'f','f'},{'g','g'},{'h','h'}}},
    {'c', {{'a','a'},{'b','c'},{'c','b'},{'e','g'},{'f','h'},{'g','e'},{'h','f'}}},
    {'e', {{'a','a'},{'b','e'},{'c','g'},{'e','f'},{'f','b'},{'g','h'},{'h','c'}}},
    {'f', {{'a','a'},{'b','f'},{'c','h'},{'e','b'},{'f','e'},{'g','c'},{'h','g'}}},
    {'g', {{'a','a'},{'b','g'},{'c','e'},{'e','h'},{'f','c'},{'g','f'},{'h','b'}}},
    {'h', {{'a','a'},{'b','h'},{'c','f'},{'e','c'},{'f','g'},{'g','b'},{'h','e'}}},
};

static const Table MULHI = {
    {'a', {{'a','a'},{'b','a'},{'c','a'},{'e','a'},{'f','a'},{'g','a'},{'h','a'}}},
    {'b', {{'a','a'},{'b','a'},{'c','a'},{'e','a'},{'f','a'},{'g','a'},{'h','a'}}},
    {'c', {{'a','a'},{'b','a'},{'c','g'},{'e','b'},{'f','h'},{'g','f'},{'h','e'}}},
    {'e', {{'a','a'},{'b','a'},{'c','b'},{'e','a'},{'f','b'},{'g','b'},{'h','a'}}},
    {'f', {{'a','a'},{'b','a'},{'c','h'},{'e','b'},{'f','e'},{'g','e'},{'h','b'}}},
    {'g', {{'a','a'},{'b','a'},{'c','f'},{'e','b'},{'f','e'},{'g','h'},{'h','e'}}},
    {'h', {{'a','a'},{'b','a'},{'c','e'},{'e','a'},{'f','b'},{'g','e'},{'h','b'}}},
};

static const DivTab DIV = {
    {'a', {{'b',{'a','a'}},{'c',{'a','a'}},{'e',{'a','a'}},{'f',{'a','a'}},{'g',{'a','a'}},{'h',{'a','a'}}}},
    {'b', {{'b',{'b','a'}},{'c',{'a','b'}},{'e',{'a','b'}},{'f',{'a','b'}},{'g',{'a','b'}},{'h',{'a','b'}}}},
    {'c', {{'b',{'c','a'}},{'c',{'b','a'}},{'e',{'h','a'}},{'f',{'b','e'}},{'g',{'b','b'}},{'h',{'e','a'}}}},
    {'e', {{'b',{'e','a'}},{'c',{'a','e'}},{'e',{'b','a'}},{'f',{'a','e'}},{'g',{'a','e'}},{'h',{'a','e'}}}},
    {'f', {{'b',{'f','a'}},{'c',{'a','f'}},{'e',{'e','a'}},{'f',{'b','a'}},{'g',{'a','f'}},{'h',{'b','b'}}}},
    {'g', {{'b',{'g','a'}},{'c',{'a','g'}},{'e',{'e','b'}},{'f',{'b','b'}},{'g',{'b','a'}},{'h',{'b','e'}}}},
    {'h', {{'b',{'h','a'}},{'c',{'a','h'}},{'e',{'b','b'}},{'f',{'a','h'}},{'g',{'a','h'}},{'h',{'b','a'}}}},
};

static map<char, map<char, bool>> LESS;

static const char ZERO = 'a';
static const char ONE  = 'b';

static char succ(char x) { return ADD.at(x).at(ONE); }

static void buildOrder() {
    string seen;                       // уже пройденные элементы цепочки
    char cur = ZERO;
    do {
        for (char s : seen) LESS[s][cur] = true;   // всё, что раньше cur, меньше cur
        seen.push_back(cur);
        cur = succ(cur);
    } while (cur != ZERO);
}//попросить примеры

static bool lessD(char x, char y) {
    auto it = LESS.find(x);
    if (it == LESS.end()) return false;
    auto jt = it->second.find(y);
    return jt != it->second.end() && jt->second;
}//попросить примеры

using Num = string;
struct SNum { bool neg; Num mag; };
enum class Cmp { LT, EQ, GT };

static const size_t MAX_DIGITS = 8;

static Num strip(Num s) {
    auto it = s.begin();
    while (it != s.end() && *it == ZERO) ++it;
    s.erase(s.begin(), it);
    if (s.empty()) s = string(1, ZERO);
    return s;
}

static Cmp cmpMag(const Num& A, const Num& B) {
    auto ia = A.rbegin();
    auto ib = B.rbegin();
    Cmp res = Cmp::EQ;
    while (ia != A.rend() && ib != B.rend()) {
        if (*ia != *ib) res = lessD(*ia, *ib) ? Cmp::LT : Cmp::GT;
        ++ia; ++ib;
    }
    if (ia != A.rend()) return Cmp::GT;
    if (ib != B.rend()) return Cmp::LT;
    return res;
}

static Num addMag(const Num& A, const Num& B) {
    Num res;
    char carry = ZERO;
    auto ia = A.rbegin();
    auto ib = B.rbegin();
    while (ia != A.rend() || ib != B.rend() || carry != ZERO) {
        char x = (ia != A.rend()) ? *ia : ZERO;
        char y = (ib != B.rend()) ? *ib : ZERO;
        char s1 = ADD.at(x).at(y);
        bool c1 = lessD(s1, x);
        char s2 = ADD.at(s1).at(carry);
        bool c2 = lessD(s2, s1);
        res.insert(res.begin(), s2);
        carry = (c1 || c2) ? ONE : ZERO;
        if (ia != A.rend()) ++ia;
        if (ib != B.rend()) ++ib;
    }
    return strip(res);
}

static Num subMag(const Num& A, const Num& B) {
    Num res;
    char borrow = ZERO;
    auto ia = A.rbegin();
    auto ib = B.rbegin();
    while (ia != A.rend()) {
        char x = *ia;
        char y = (ib != B.rend()) ? *ib : ZERO;
        char d1 = SUB.at(x).at(y);
        bool b1 = lessD(x, y);
        char d2 = SUB.at(d1).at(borrow);
        bool b2 = lessD(d1, borrow);
        res.insert(res.begin(), d2);
        borrow = (b1 || b2) ? ONE : ZERO;
        ++ia;
        if (ib != B.rend()) ++ib;
    }
    return strip(res);
}

// Умножение модуля на одну цифру: младшая цифра из MUL, перенос - из MULHI.
static Num mulDigit(const Num& A, char y) {
    Num res;
    char carry = ZERO;
    for (auto ia = A.rbegin(); ia != A.rend(); ++ia) {
        char lo = MUL.at(*ia).at(y);
        char hi = MULHI.at(*ia).at(y);
        char t  = ADD.at(lo).at(carry);
        if (lessD(t, lo)) hi = succ(hi);       // сложение с переносом дало ещё одну единицу
        res.insert(res.begin(), t);
        carry = hi;
    }
    if (carry != ZERO) res.insert(res.begin(), carry);
    return strip(res);
}

// Умножение «в столбик».
static Num mulMag(const Num& A, const Num& B) {
    Num total(1, ZERO);
    std::string shift;                         // сдвиг на разряд = дописать 'a' справа
    for (auto ib = B.rbegin(); ib != B.rend(); ++ib) {
        Num part = mulDigit(A, *ib);
        if (part != std::string(1, ZERO)) part += shift;
        total = addMag(total, part);
        shift.push_back(ZERO);
    }
    return total;
}

struct QR { Num q, r; };

// Деление модулей с остатком (D != 0). Цифра частного = сколько раз вычли делитель.
static QR divMag(const Num& N, const Num& D) {
    if (N.size() == 1 && D.size() == 1) {      // обе - цифры: берём из таблицы DIV
        auto qr = DIV.at(N[0]).at(D[0]);
        return { std::string(1, qr.first), std::string(1, qr.second) };
    }
    Num q;
    Num R(1, ZERO);
    for (char d : N) {
        if (R == std::string(1, ZERO)) R = std::string(1, d);   // R = R*база + d : дописываем цифру справа
        else R.push_back(d);
        char cnt = ZERO;
        while (cmpMag(R, D) != Cmp::LT) {
            R = subMag(R, D);
            cnt = succ(cnt);
        }
        q.push_back(cnt);
    }
    return { strip(q), R };
}

// ============================================================================
//  Операции со знаком
// ============================================================================
static bool isZero(const Num& m) { return m == std::string(1, ZERO); }

static SNum fixZero(SNum v) { if (isZero(v.mag)) v.neg = false; return v; }

static SNum addS(const SNum& x, const SNum& y) {
    SNum r;
    if (x.neg == y.neg) {
        r = { x.neg, addMag(x.mag, y.mag) };
    } else {
        Cmp c = cmpMag(x.mag, y.mag);
        if (c == Cmp::EQ)      r = { false, std::string(1, ZERO) };
        else if (c == Cmp::GT) r = { x.neg, subMag(x.mag, y.mag) };
        else                   r = { y.neg, subMag(y.mag, x.mag) };
    }
    return fixZero(r);
}

static SNum subS(const SNum& x, const SNum& y) {
    return addS(x, SNum{ !y.neg, y.mag });
}

static SNum mulS(const SNum& x, const SNum& y) {
    return fixZero(SNum{ x.neg != y.neg, mulMag(x.mag, y.mag) });
}

// Целочисленное деление, остаток всегда неотрицательный: n = q*m + r, 0 <= r < |m|.
// Возвращает false при делении на a (ноль).
static bool divS(const SNum& n, const SNum& m, SNum& q, SNum& r) {
    if (isZero(m.mag)) return false;
    QR qr = divMag(n.mag, m.mag);
    Num Q = qr.q;
    Num rem = qr.r;
    if (n.neg && !isZero(rem)) {               // делимое отрицательное и не делится нацело:
        Q   = addMag(Q, std::string(1, ONE));  //   округляем частное «вниз»,
        rem = subMag(m.mag, rem);              //   остаток = |m| - остаток
    }
    q = fixZero(SNum{ n.neg != m.neg, Q });
    r = SNum{ false, rem };
    return true;
}

// ============================================================================
//  Ввод и вывод
// ============================================================================
static char canon(char ch) { return ch == 'd' ? 'c' : ch; }    // d равносильно c

// Разбор операнда: [знак] (1 | до 8 букв a..h). Возвращает "" при успехе, иначе текст ошибки.
static std::string parseOperand(const std::string& tok, SNum& out) {
    auto it = tok.begin();
    bool neg = false;
    if (it != tok.end() && (*it == '+' || *it == '-')) { neg = (*it == '-'); ++it; }
    std::string body(it, tok.end());
    if (body.empty()) return "после знака нет числа";
    if (body == "1") {                          // допустимая цифра: «+1» / «-1» (то есть b)
        out = { neg, std::string(1, ONE) };
        return "";
    }
    static const std::string LETTERS = "abcdefgh";
    Num mag;
    for (char raw : body) {
        char ch = (char)std::tolower((unsigned char)raw);
        if (std::isdigit((unsigned char)ch))
            return std::string("цифра '") + raw + "' недопустима (из цифр можно только 1 как +1 / -1)";
        if (LETTERS.find(ch) == std::string::npos)
            return std::string("символ '") + raw + "' недопустим (нужны буквы a b c d e f g h)";
        mag.push_back(canon(ch));
    }
    if (mag.size() > MAX_DIGITS) return "в числе больше 8 разрядов";
    mag = strip(mag);
    out = { neg && !isZero(mag), mag };
    return "";
}

static std::string render(const SNum& v, char cd) {
    std::string s = v.neg ? "-" : "";
    for (char ch : v.mag) s.push_back(ch == 'c' ? cd : ch);
    return s;
}

// Если в записи есть c, печатаем запись с c = запись с d.
static std::string both(const SNum& v) {
    if (v.mag.find('c') == std::string::npos) return render(v, 'c');
    return render(v, 'c') + " = " + render(v, 'd');
}

static void warnOverflow(const SNum& v) {
    if (v.mag.size() > MAX_DIGITS) std::cout << "   (внимание: результат длиннее 8 разрядов - переполнение разрядной сетки)\n";
}

// Вычисление одной строки. Возвращает "" при успехе или текст ошибки формата.
static std::string evalLine(const std::string& line) {
    std::istringstream in(line);
    std::string t1, op, t2, extra;
    if (!(in >> t1 >> op >> t2) || (in >> extra))
        return "формат: <число> <операция> <число>, между числами и операцией обязательны пробелы";
    if (op != "+" && op != "-" && op != "*" && op != "/")
        return "операция должна быть одной из: + - * /";

    SNum x, y;
    std::string err = parseOperand(t1, x);
    if (!err.empty()) return "первое число: " + err;
    err = parseOperand(t2, y);
    if (!err.empty()) return "второе число: " + err;

    if (op == "/") {
        SNum q, r;
        if (!divS(x, y, q, r)) {
            std::cout << "Ошибка: деление на a (ноль) невозможно.\n";
            return "";
        }
        std::cout << "Частное: " << both(q) << "\n";
        std::cout << "Остаток: " << both(r) << "\n";
        warnOverflow(q);
        return "";
    }
    SNum res = (op == "+") ? addS(x, y) : (op == "-") ? subS(x, y) : mulS(x, y);
    std::cout << "Результат: " << both(res) << "\n";
    warnOverflow(res);
    return "";
}

static void printTable(const std::string& title, const Table& t) {
    std::cout << "\n" << title << "\n   |";
    for (const auto& kv : t.begin()->second) std::cout << ' ' << kv.first;
    std::cout << "\n";
    for (const auto& row : t) {
        std::cout << " " << row.first << " |";
        for (const auto& cell : row.second) std::cout << ' ' << cell.second;
        std::cout << "\n";
    }
}

static void printDiv() {
    std::cout << "\nДеление (частное,остаток): строка - делимое, столбец - делитель\n   |";
    for (const auto& kv : DIV.begin()->second) std::cout << "  " << kv.first << " ";
    std::cout << "\n";
    for (const auto& row : DIV) {
        std::cout << " " << row.first << " |";
        for (const auto& cell : row.second) std::cout << ' ' << cell.second.first << ',' << cell.second.second;
        std::cout << "\n";
    }
}

static void showTables() {
    printTable("Сложение (строка + столбец)", ADD);
    printTable("Вычитание (строка - столбец)", SUB);
    printTable("Умножение, младшая цифра (строка * столбец)", MUL);
    printTable("Умножение, старшая цифра (перенос)", MULHI);
    printDiv();
    std::cout << "\nЭлемент d равносилен c.\n";
}

static void calcLoop() {
    std::string line;
    while (true) {
        std::cout << "\nВведите выражение (например: b + -a, -a + b, c * 1, bc / e), m - в меню: ";
        if (!std::getline(std::cin, line)) return;
        if (line == "m" || line == "M") return;
        std::string err = evalLine(line);
        if (!err.empty()) std::cout << "Ошибка ввода: " << err << ". Введите снова.\n";
    }
}

int main() {
    buildOrder();
    std::string line;
    while (true) {
        std::cout << "\n=== Конечная арифметика, вариант X8 ===\n"
                     "1 - вычислить выражение\n"
                     "2 - показать таблицы\n"
                     "0 - выход\n"
                     "Выбор: ";
        if (!std::getline(std::cin, line)) break;
        if (line == "1")      calcLoop();
        else if (line == "2") showTables();
        else if (line == "0") break;
        else std::cout << "Неизвестный пункт меню, введите 1, 2 или 0.\n";
    }
    return 0;
}