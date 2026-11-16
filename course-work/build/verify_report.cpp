#define main coursework_main
#include "../main.cpp"
#undef main
#include <random>
#include <stdexcept>

static const std::string digits = "abehfgc";
static Num encode(long long n) {
    if (n == 0) return "a";
    Num result;
    while (n) { result.insert(result.begin(), digits[n % 7]); n /= 7; }
    return result;
}
static SNum signedNumber(long long n) { return {n < 0, encode(n < 0 ? -n : n)}; }
static long long decode(const SNum& n) {
    long long result = 0;
    for (char c : n.mag) result = result * 7 + digits.find(c);
    return n.neg ? -result : result;
}
static void check(bool ok) { if (!ok) throw std::runtime_error("verification failed"); }
int main() {
    buildOrder();
    int tableChecks = 0;
    for (int x = 0; x < 7; ++x) for (int y = 0; y < 7; ++y) {
        char a = digits[x], b = digits[y];
        check(ADD.at(a).at(b) == digits[(x+y)%7]);
        check(SUB.at(a).at(b) == digits[(x-y+7)%7]);
        check(MUL.at(a).at(b) == digits[(x*y)%7]);
        check(MULHI.at(a).at(b) == digits[(x*y)/7]);
        tableChecks += 4;
        if (y) {
            check(DIV.at(a).at(b) == std::make_pair(digits[x/y], digits[x%y]));
            ++tableChecks;
        }
    }
    std::mt19937_64 generator(20261116);
    const long long limit = 5764800; // 7^8 - 1
    std::uniform_int_distribution<long long> dist(-limit, limit);
    int arithmeticChecks = 0;
    auto verify = [&](long long x, long long y) {
        SNum a = signedNumber(x), b = signedNumber(y), q, r;
        check(decode(addS(a,b)) == x+y);
        check(decode(subS(a,b)) == x-y);
        check(decode(mulS(a,b)) == x*y);
        arithmeticChecks += 3;
        if (y) {
            check(divS(a,b,q,r));
            long long quotient = decode(q), remainder = decode(r);
            check(x == quotient*y + remainder && remainder >= 0 && remainder < (y<0?-y:y));
        } else check(!divS(a,b,q,r));
        ++arithmeticChecks;
    };
    for (int x=-48; x<=48; ++x) for (int y=-48; y<=48; ++y) verify(x,y);
    for (int i=0; i<10000; ++i) verify(dist(generator),dist(generator));
    int inputChecks=0;
    for (const auto& item : std::map<std::string,std::string>{{"D","c"},{"aabd","bc"},{"-a","a"},{"-1","-b"},{"+1","b"},{"1","b"},{"CcDd","cccc"}}) {
        SNum n; check(parseOperand(item.first,n).empty()); check(render(n,'c')==item.second); ++inputChecks;
    }
    for (const auto& s : {"", "-", "2", "b1", "i", "aaaaaaaaa", "-abcdefgha"}) {
        SNum n; check(!parseOperand(s,n).empty()); ++inputChecks;
    }
    check(addMag("cccccccc","b")=="baaaaaaaa");
    check(mulMag("cccccccc","cccccccc")==encode(limit*limit));
    std::cout << "table_entries=" << tableChecks << "\n"
              << "operand_pairs=" << 97*97+10000 << "\n"
              << "arithmetic_checks=" << arithmeticChecks << "\n"
              << "input_checks=" << inputChecks << "\nPASS\n";
}
