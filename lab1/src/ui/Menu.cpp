#include "ui/Menu.hpp"
#include "core/Random.hpp"
#include "core/Universe.hpp"
#include "ui/Input.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>


static const size_t TABLE_LIMIT = 32;    // наибольшее число строк таблицы
static const size_t PRINT_LIMIT = 16;    // наибольшее число элементов мультимножества и слов кода в выводе
static const size_t LINE_WIDTH = 100;    // примерная наибольшая ширина записи мультимножества при n > 4
static const size_t MATRIX_WIDTH = 112;  // наибольшая ширина строки матрицы кратностей

// операции в порядке вывода; результаты compute_operations идут в том же порядке
struct OperationInfo{
    const char* title;      // название
    const char* notation;   // обозначение
    const char* formula;    // кратность элемента в результате
};

static const OperationInfo OPERATIONS[] = {
    {"объединение", "A ∪ B", "max(kA, kB)"},
    {"пересечение", "A ∩ B", "min(kA, kB)"},
    {"разность", "A \\ B", "min(kA, kU - kB)"},
    {"разность", "B \\ A", "min(kB, kU - kA)"},
    {"симметрическая разность", "A ∆ B", "max(k[A\\B], k[B\\A])"},
    {"дополнение", "¬A", "kU - kA"},
    {"дополнение", "¬B", "kU - kB"},
    {"арифметическая сумма", "A + B", "min(kA + kB, kU)"},
    {"арифметическая разность", "A - B", "max(kA - kB, 0)"},
    {"арифметическая разность", "B - A", "max(kB - kA, 0)"},
    {"арифметическое произведение", "A * B", "min(kA * kB, kU)"},
    {"арифметическое деление", "A / B", "kA div kB; 0 при kB = 0"},
    {"арифметическое деление", "B / A", "kB div kA; 0 при kA = 0"},
};
static const size_t OPERATION_COUNT = sizeof(OPERATIONS) / sizeof(OPERATIONS[0]);

static const char* COMMANDS = "u - универсум, a/b - задать A/B, t - таблица, o - операции, "
                              "m - матрица, h - справка, q - выход";


// ширина строки в символах: в UTF-8 продолжения символа имеют вид 10xxxxxx
static size_t text_width(const std::string& text){
    size_t width = 0;
    for(size_t i = 0; i < text.size(); i++){
        if((static_cast<unsigned char>(text[i]) & 0xC0) != 0x80) width++;
    }
    return width;
}

static std::string pad_right(const std::string& text, size_t width){
    size_t current = text_width(text);
    return current >= width ? text : text + std::string(width - current, ' ');
}

static std::string pad_left(const std::string& text, size_t width){
    size_t current = text_width(text);
    return current >= width ? text : std::string(width - current, ' ') + text;
}

// при n <= 4 запись полная (не больше 2^4 = PRINT_LIMIT элементов); при больших n элементов
// столько, сколько помещается примерно в LINE_WIDTH символов, но не меньше 4
static std::string format_multiset(const Multiset& multiset){
    size_t depth = multiset.get_carrier().get_depth();
    size_t limit = PRINT_LIMIT;
    if(depth > 4){
        size_t element_width = depth + 6;   // "код×100, "
        limit = std::min(PRINT_LIMIT, std::max<size_t>(4, LINE_WIDTH / element_width));
    }
    std::ostringstream out;
    print_multiset(out, multiset, limit);
    return out.str();
}

// обозначение без пробелов для заголовков столбцов: A ∪ B -> A∪B
static std::string compact(const std::string& notation){
    std::string res = notation;
    res.erase(std::remove(res.begin(), res.end(), ' '), res.end());
    return res;
}


Menu::Menu():carrier(std::make_shared<CodeSet>(0)),universe(carrier),a(carrier),b(carrier),
             universe_ready(false),a_ready(false),b_ready(false){}

void Menu::run(){
    // список команд выводится перед каждым приглашением
    while(true){
        std::cout << std::endl << COMMANDS << std::endl;
        char command = read_letter("> ", "uabtomhq", false);
        if(command == 'q') return;
        if(!is_available(command)) continue;
        switch(command){
        case 'u': create_universe(); break;
        case 'a': fill_multiset(a, a_ready, "A"); break;
        case 'b': fill_multiset(b, b_ready, "B"); break;
        case 't': show_table(); break;
        case 'o': show_operations(); break;
        case 'm': show_matrix(); break;
        case 'h': show_help(); break;
        }
    }
}

// печатает, чего не хватает для команды, и возвращает false, если команду выполнить нельзя
bool Menu::is_available(char command) const{
    if(command == 'u' || command == 'h') return true;
    if(!universe_ready){
        std::cout << "! команда " << command << " недоступна: универсум ещё не создан (команда u)" << std::endl;
        return false;
    }
    if((command == 'o' || command == 'm') && (!a_ready || !b_ready)){
        std::string missing;
        if(!a_ready) missing = "A (команда a)";
        if(!b_ready) missing += std::string(missing.empty() ? "" : " и ") + "B (команда b)";
        std::cout << "! команда " << command << " недоступна: не задано " << missing << std::endl;
        return false;
    }
    return true;
}

void Menu::create_universe(){
    size_t depth = read_number("n [0.." + std::to_string(MAX_DEPTH) + "]: ", 0, MAX_DEPTH);
    std::shared_ptr<const CodeSet> new_carrier = std::make_shared<CodeSet>(depth);
    size_t size = new_carrier->size();
    Multiset new_universe(new_carrier);
    size_t multiplicity = 0;

    if(size == 0){
        // элементов нет, поэтому и кратность задавать некому
        std::cout << "при n = 0 в коде Грея нет слов, кратность не нужна" << std::endl;
    }
    else{
        if(!read_number_or_empty("k [1.." + std::to_string(MAX_MULTIPLICITY) + ", Enter - случайно]: ",
                                 1, MAX_MULTIPLICITY, multiplicity)){
            multiplicity = 1 + random_below(MAX_MULTIPLICITY);
            std::cout << "k = " << multiplicity << std::endl;
        }
        new_universe = uniform_universe(new_carrier, multiplicity);
    }

    bool had_multisets = a_ready || b_ready;
    carrier = new_carrier;
    universe = new_universe;
    a = Multiset(carrier);
    b = Multiset(carrier);
    universe_ready = true;
    a_ready = false;
    b_ready = false;

    // не больше PRINT_LIMIT слов кода, в строке столько, сколько помещается в LINE_WIDTH
    if(size > 0){
        size_t shown = std::min(size, PRINT_LIMIT);
        size_t per_line = std::max<size_t>(1, LINE_WIDTH / (depth + 1));
        std::cout << "код Грея, 2^" << depth << " = " << size << ":";
        for(size_t i = 0; i < shown; i++){
            if(shown > per_line && i % per_line == 0) std::cout << std::endl;
            else if(i > 0 || shown <= per_line) std::cout << " ";
            std::cout << carrier->get_code(i);
        }
        std::cout << std::endl;
        if(shown < size) std::cout << "... и ещё " << size - shown << std::endl;
    }
    std::cout << "U = " << format_multiset(universe) << ", |U| = " << universe.cardinality() << std::endl;
    if(had_multisets) std::cout << "A и B сброшены" << std::endl;
}

void Menu::fill_multiset(Multiset& multiset, bool& ready, const std::string& name){
    size_t high = universe.cardinality();
    size_t cardinality = 0;
    if(!read_number_or_empty("|" + name + "| [0.." + std::to_string(high) + ", Enter - случайно]: ",
                             0, high, cardinality)){
        cardinality = random_below(high + 1);
        std::cout << "|" << name << "| = " << cardinality << std::endl;
    }
    char method = read_letter("r - случайно, m - вручную, Enter - отмена: ", "rm", true);
    if(method == '\0'){
        std::cout << "отменено" << std::endl;
        return;
    }

    // заполняется копия: при отмене ручного ввода прежнее мультимножество сохраняется
    Multiset filled(carrier);
    if(method == 'm'){
        if(!fill_manual(filled, cardinality, name)){
            std::cout << "отменено" << std::endl;
            return;
        }
    }
    else{
        filled = random_submultiset(universe, cardinality);
    }

    multiset = filled;
    ready = true;
    std::cout << name << " = " << format_multiset(multiset) << ", |" << name << "| = "
              << multiset.cardinality() << std::endl;
}

bool Menu::fill_manual(Multiset& target, size_t cardinality, const std::string& name) const{
    if(cardinality == 0) return true;
    std::cout << "пары код:кратность через пробел (01:2 11), каждого кода не больше "
              << universe.get_count(0) << ", Enter - отмена" << std::endl;
    while(target.cardinality() < cardinality){
        size_t left = cardinality - target.cardinality();
        std::string line = read_text(name + " = " + format_multiset(target) + ", осталось " + std::to_string(left) + ": ");
        if(line.empty()) return false;
        std::vector<size_t> indices;
        std::vector<size_t> counts;
        // строка применяется только целиком: при ошибке в любой паре A не меняется
        if(parse_manual_line(line, target, left, indices, counts)){
            for(size_t i = 0; i < indices.size(); i++){
                target.set_count(indices[i], target.get_count(indices[i]) + counts[i]);
            }
        }
    }
    return true;
}

// разбирает строку пар код:кратность; при ошибке печатает её причину и возвращает false
bool Menu::parse_manual_line(const std::string& line, const Multiset& target, size_t left,
                             std::vector<size_t>& indices, std::vector<size_t>& counts) const{
    size_t depth = carrier->get_depth();
    size_t total = 0;
    std::istringstream words(line);
    std::string word;
    while(words >> word){
        size_t colon = word.find(':');
        std::string code = word.substr(0, colon);
        size_t count = 1;
        if(!is_code(code, depth)){
            std::cout << "! «" << word << "»: код должен состоять из " << depth << " символов 0 и 1" << std::endl;
            return false;
        }
        if(colon != std::string::npos && (!parse_number(word.substr(colon + 1), count) || count == 0)){
            std::cout << "! «" << word << "»: после двоеточия нужна кратность - целое число от 1" << std::endl;
            return false;
        }
        size_t index = 0;
        carrier->find(code, index);   // код длины n из 0 и 1 всегда есть в носителе
        // учитываются и единицы этого же кода, уже записанные в строке раньше
        size_t taken = target.get_count(index);
        for(size_t i = 0; i < indices.size(); i++){
            if(indices[i] == index) taken += counts[i];
        }
        size_t free = universe.get_count(index) - std::min(taken, universe.get_count(index));
        if(count > free){
            std::cout << "! «" << word << "»: у " << code << " свободно " << free << " (kU = "
                      << universe.get_count(index) << ", занято " << taken << ")" << std::endl;
            return false;
        }
        indices.push_back(index);
        counts.push_back(count);
        total += count;
    }
    if(total > left){
        std::cout << "! в строке " << total << " ед., а осталось распределить " << left << std::endl;
        return false;
    }
    return true;
}

std::vector<Multiset> Menu::compute_operations() const{
    std::vector<Multiset> res;
    res.push_back(a.unite(b));
    res.push_back(a.intersect(b));
    res.push_back(a.difference(b, universe));
    res.push_back(b.difference(a, universe));
    res.push_back(a.symmetric_difference(b, universe));
    res.push_back(a.complement(universe));
    res.push_back(b.complement(universe));
    res.push_back(a.arithmetic_sum(b, universe));
    res.push_back(a.arithmetic_difference(b));
    res.push_back(b.arithmetic_difference(a));
    res.push_back(a.arithmetic_product(b, universe));
    res.push_back(a.arithmetic_division(b));
    res.push_back(b.arithmetic_division(a));
    return res;
}

void Menu::show_help() const{
    std::cout << "запись {000×2, 011×1}: код Грея × кратность, {} - пустое; kU, kA, kB - кратности в U, A, B" << std::endl;
    for(size_t i = 0; i < OPERATION_COUNT; i++){
        std::cout << pad_right(OPERATIONS[i].notation, 7) << pad_right(OPERATIONS[i].formula, 26)
                  << OPERATIONS[i].title << std::endl;
    }
    std::cout << "0 <= n <= " << MAX_DEPTH << ", 1 <= k <= " << MAX_MULTIPLICITY << ", 0 <= |A|, |B| <= |U|" << std::endl;
}

void Menu::show_table() const{
    size_t code_width = std::max<size_t>(carrier->get_depth(), 3);
    // числа выравниваются по самому длинному из них - мощности |U| в строке Σ
    size_t number_width = std::max<size_t>(std::to_string(universe.cardinality()).size(), 3) + 1;
    std::cout << pad_right("код", code_width) << pad_left("kU", number_width)
              << pad_left("kA", number_width) << pad_left("kB", number_width) << std::endl;
    size_t rows = std::min(universe.size(), TABLE_LIMIT);
    for(size_t i = 0; i < rows; i++){
        std::cout << pad_right(carrier->get_code(i), code_width)
                  << pad_left(std::to_string(universe.get_count(i)), number_width)
                  << pad_left(a_ready ? std::to_string(a.get_count(i)) : "-", number_width)
                  << pad_left(b_ready ? std::to_string(b.get_count(i)) : "-", number_width) << std::endl;
    }
    if(rows < universe.size()){
        std::cout << "... показаны " << rows << " из " << universe.size() << std::endl;
    }
    std::cout << pad_right("Σ", code_width)
              << pad_left(std::to_string(universe.cardinality()), number_width)
              << pad_left(a_ready ? std::to_string(a.cardinality()) : "-", number_width)
              << pad_left(b_ready ? std::to_string(b.cardinality()) : "-", number_width) << std::endl;
}

void Menu::show_operations() const{
    std::vector<Multiset> results = compute_operations();
    for(size_t i = 0; i < OPERATION_COUNT; i++){
        std::cout << pad_right(OPERATIONS[i].notation, 6) << "= " << format_multiset(results[i])
                  << ", |" << OPERATIONS[i].notation << "| = " << results[i].cardinality() << std::endl;
    }

    // следствия определений: |¬A| = |U| - |A| и |A ∪ B| + |A ∩ B| = |A| + |B|
    size_t u = universe.cardinality();
    size_t na = a.cardinality();
    size_t nb = b.cardinality();
    size_t complement = results[5].cardinality();
    size_t unite = results[0].cardinality();
    size_t intersect = results[1].cardinality();
    std::cout << "проверка: |¬A| = |U| - |A|: " << complement << " = " << u << " - " << na
              << (complement == u - na ? " верно" : " НЕВЕРНО") << std::endl;
    std::cout << "проверка: |A ∪ B| + |A ∩ B| = |A| + |B|: " << unite << " + " << intersect << " = " << na << " + " << nb
              << (unite + intersect == na + nb ? " верно" : " НЕВЕРНО") << std::endl;
}

void Menu::show_matrix() const{
    std::vector<Multiset> results = compute_operations();
    std::vector<std::string> titles = {"U", "A", "B"};
    std::vector<const Multiset*> rows = {&universe, &a, &b};
    for(size_t i = 0; i < OPERATION_COUNT; i++){
        titles.push_back(compact(OPERATIONS[i].notation));
        rows.push_back(&results[i]);
    }

    // столбцов столько, сколько помещается в MATRIX_WIDTH
    const size_t label_width = 4;
    const size_t total_width = 9;
    size_t column_width = std::max<size_t>(carrier->get_depth(), 3) + 2;
    size_t columns = std::min(universe.size(), (MATRIX_WIDTH - label_width - total_width) / column_width);

    std::cout << pad_right("", label_width);
    for(size_t j = 0; j < columns; j++) std::cout << pad_left(carrier->get_code(j), column_width);
    std::cout << pad_left("|·|", total_width) << std::endl;
    for(size_t i = 0; i < rows.size(); i++){
        std::cout << pad_right(titles[i], label_width);
        for(size_t j = 0; j < columns; j++){
            std::cout << pad_left(std::to_string(rows[i]->get_count(j)), column_width);
        }
        std::cout << pad_left(std::to_string(rows[i]->cardinality()), total_width) << std::endl;
    }
    if(columns < universe.size()){
        std::cout << "... показаны " << columns << " из " << universe.size() << " кодов, мощности по всем" << std::endl;
    }
}
