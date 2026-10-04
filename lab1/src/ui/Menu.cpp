#include "ui/Menu.hpp"
#include "core/Random.hpp"
#include "core/Universe.hpp"
#include "ui/Input.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>


static const size_t TABLE_LIMIT = 32;   // наибольшее число строк таблицы на экране
static const size_t PRINT_LIMIT = 16;   // наибольшее число элементов мультимножества в строке

static const std::string LINE = "==================================================";
static const std::string THIN_LINE = "--------------------------------------------------";


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

static std::string format_multiset(const Multiset& multiset){
    std::ostringstream out;
    print_multiset(out, multiset, PRINT_LIMIT);
    return out.str();
}

static void print_header(const std::string& title){
    std::cout << "--- " << title << " ---" << std::endl;
}


Menu::Menu():carrier(std::make_shared<CodeSet>(0)),universe(carrier),a(carrier),b(carrier),
             universe_ready(false),a_ready(false),b_ready(false){}

void Menu::run(){
    while(true){
        print_menu();
        size_t choice = read_number("Выберите пункт (0..6): ", 0, 6);
        std::cout << std::endl;
        switch(choice){
        case 0:
            std::cout << "Работа завершена." << std::endl;
            return;
        case 1:
            create_universe();
            break;
        case 2:
            if(require_universe()) fill_multiset(a, a_ready, "A");
            break;
        case 3:
            if(require_universe()) fill_multiset(b, b_ready, "B");
            break;
        case 4:
            if(require_universe()) show_table(true);
            break;
        case 5:
            if(require_universe() && require_multisets()) show_operations();
            break;
        case 6:
            if(require_universe() && require_multisets()) show_operations_table();
            break;
        }
        std::cout << std::endl;
    }
}

void Menu::print_menu() const{
    std::cout << LINE << std::endl;
    std::cout << "      Мультимножества на бинарном коде Грея" << std::endl;
    std::cout << LINE << std::endl;
    std::cout << "  U: ";
    if(universe_ready){
        std::cout << "n = " << carrier->get_depth() << ", элементов " << universe.size()
                  << ", |U| = " << universe.cardinality();
        if(universe.size() == 0) std::cout << " (U = {})";
        std::cout << std::endl;
    }
    else{
        std::cout << "не создан" << std::endl;
    }
    std::cout << "  A: ";
    if(a_ready) std::cout << "|A| = " << a.cardinality() << std::endl;
    else std::cout << "не заполнено" << std::endl;
    std::cout << "  B: ";
    if(b_ready) std::cout << "|B| = " << b.cardinality() << std::endl;
    else std::cout << "не заполнено" << std::endl;
    std::cout << THIN_LINE << std::endl;
    std::cout << "  1. Создать универсум U" << std::endl;
    std::cout << "  2. Заполнить A" << std::endl;
    std::cout << "  3. Заполнить B" << std::endl;
    std::cout << "  4. Таблица кратностей U, A, B" << std::endl;
    std::cout << "  5. Операции над A и B" << std::endl;
    std::cout << "  6. Таблица кратностей результатов операций" << std::endl;
    std::cout << "  0. Выход" << std::endl;
    std::cout << THIN_LINE << std::endl;
}

bool Menu::require_universe() const{
    if(!universe_ready){
        std::cout << "Сначала создайте универсум (пункт 1)." << std::endl;
        return false;
    }
    return true;
}

bool Menu::require_multisets() const{
    if(!a_ready || !b_ready){
        std::cout << "Сначала заполните A и B (пункты 2 и 3)." << std::endl;
        return false;
    }
    return true;
}

void Menu::create_universe(){
    print_header("Создание универсума");
    size_t depth = read_number("Разрядность кода Грея n (0.." + std::to_string(MAX_DEPTH) + "): ", 0, MAX_DEPTH);
    std::shared_ptr<const CodeSet> new_carrier = std::make_shared<CodeSet>(depth);
    size_t size = new_carrier->size();
    Multiset new_universe(new_carrier);

    if(size == 0){
        std::cout << "При n = 0 код Грея не содержит ни одного слова: U = {}, |U| = 0." << std::endl;
    }
    else{
        std::cout << "Код Грея построен, число элементов 2^n = " << size << "." << std::endl;
        std::cout << "Способ задания кратностей k_U(x):" << std::endl;
        std::cout << "  1 - случайно, от 1 до K" << std::endl;
        std::cout << "  2 - вручную, для каждого элемента" << std::endl;
        std::cout << "  3 - по мощности |U|, кратности распределяются случайно" << std::endl;
        std::cout << "  0 - отмена" << std::endl;
        size_t method = read_number("Способ (0..3): ", 0, 3);
        if(method == 0){
            std::cout << "Создание отменено, прежнее состояние сохранено." << std::endl;
            return;
        }
        if(method == 1){
            size_t max_multiplicity = read_number("Наибольшая кратность K (1.." + std::to_string(MAX_MULTIPLICITY) + "): ",
                                                  1, MAX_MULTIPLICITY);
            new_universe = random_universe(new_carrier, max_multiplicity);
        }
        else if(method == 2){
            if(!read_universe_manually(new_universe)){
                std::cout << "Создание отменено, прежнее состояние сохранено." << std::endl;
                return;
            }
        }
        else{
            size_t low = min_universe_cardinality(size);
            size_t high = max_universe_cardinality(size);
            size_t cardinality = 0;
            if(!read_number_or_empty("Мощность |U| (" + std::to_string(low) + ".." + std::to_string(high)
                                     + "; Enter - случайная): ", low, high, cardinality)){
                cardinality = low + random_below(high - low + 1);
                std::cout << "Мощность выбрана случайно: |U| = " << cardinality << std::endl;
            }
            new_universe = universe_with_cardinality(new_carrier, cardinality);
        }
    }

    bool had_multisets = a_ready || b_ready;
    carrier = new_carrier;
    universe = new_universe;
    a = Multiset(carrier);
    b = Multiset(carrier);
    universe_ready = true;
    a_ready = false;
    b_ready = false;

    std::cout << "Универсум создан." << std::endl;
    if(had_multisets){
        std::cout << "A и B были построены над прежним универсумом и очищены." << std::endl;
    }
    show_table(false);
}

bool Menu::read_universe_manually(Multiset& target) const{
    std::cout << "Введите кратность каждого элемента (1.." << MAX_MULTIPLICITY
              << "); пустая строка - отмена." << std::endl;
    for(size_t i = 0; i < target.size(); i++){
        size_t count = 0;
        if(!read_number_or_empty("  k_U(" + target.get_carrier().get_code(i) + ") = ", 1, MAX_MULTIPLICITY, count)){
            return false;
        }
        target.set_count(i, count);
    }
    return true;
}

void Menu::fill_multiset(Multiset& multiset, bool& ready, const std::string& name){
    print_header("Заполнение " + name);
    std::cout << "Способ заполнения:" << std::endl;
    std::cout << "  1 - вручную" << std::endl;
    std::cout << "  2 - автоматически" << std::endl;
    std::cout << "  0 - отмена" << std::endl;
    size_t method = read_number("Способ (0..2): ", 0, 2);
    if(method == 0){
        std::cout << "Заполнение отменено, " << name << " не изменено." << std::endl;
        return;
    }

    size_t high = universe.cardinality();
    size_t cardinality = 0;
    if(!read_number_or_empty("Мощность |" + name + "| (0.." + std::to_string(high) + "; Enter - случайная): ",
                             0, high, cardinality)){
        cardinality = random_below(high + 1);
        std::cout << "Мощность выбрана случайно: |" << name << "| = " << cardinality << std::endl;
    }

    // заполняется копия: при отмене ручного ввода прежнее мультимножество сохраняется
    Multiset filled(carrier);
    if(method == 1){
        if(!fill_manual(filled, cardinality, name)){
            std::cout << "Заполнение отменено, " << name << " не изменено." << std::endl;
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
    show_table(false);
    std::cout << "Вводите код элемента и добавляемую кратность; пустая строка вместо кода - отмена." << std::endl;
    size_t depth = carrier->get_depth();
    while(target.cardinality() < cardinality){
        size_t left = cardinality - target.cardinality();
        std::cout << "Осталось распределить: " << left << std::endl;

        std::string code;
        if(!read_code_or_empty("  Код элемента: ", depth, code)){
            return false;
        }
        size_t index = 0;
        carrier->find(code, index);   // код длины n из 0 и 1 всегда есть в универсуме
        size_t free = universe.get_count(index) - target.get_count(index);
        if(free == 0){
            std::cout << "  Ошибка: элемент " << code << " уже входит в " << name << " с наибольшей кратностью k_U("
                      << code << ") = " << universe.get_count(index) << std::endl;
            continue;
        }
        size_t limit = std::min(free, left);
        size_t count = read_number("  Кратность (1.." + std::to_string(limit) + "): ", 1, limit);
        target.set_count(index, target.get_count(index) + count);
    }
    return true;
}

std::vector<Menu::Operation> Menu::compute_operations() const{
    std::vector<Operation> res;
    res.push_back({"Объединение", "A ∪ B", a.unite(b)});
    res.push_back({"Пересечение", "A ∩ B", a.intersect(b)});
    res.push_back({"Разность", "A \\ B", a.difference(b, universe)});
    res.push_back({"Разность", "B \\ A", b.difference(a, universe)});
    res.push_back({"Симметрическая разность", "A ∆ B", a.symmetric_difference(b, universe)});
    res.push_back({"Дополнение", "¬A", a.complement(universe)});
    res.push_back({"Дополнение", "¬B", b.complement(universe)});
    res.push_back({"Арифметическая сумма", "A + B", a.arithmetic_sum(b, universe)});
    res.push_back({"Арифметическая разность", "A - B", a.arithmetic_difference(b)});
    res.push_back({"Арифметическая разность", "B - A", b.arithmetic_difference(a)});
    res.push_back({"Арифметическое произведение", "A * B", a.arithmetic_product(b, universe)});
    res.push_back({"Арифметическое деление", "A / B", a.arithmetic_division(b)});
    res.push_back({"Арифметическое деление", "B / A", b.arithmetic_division(a)});
    return res;
}

void Menu::show_table(bool with_multisets) const{
    size_t depth = carrier->get_depth();
    size_t code_width = std::max<size_t>(depth, 3);
    std::cout << pad_left("№", 8) << "   " << pad_right("Код", code_width) << pad_left("k_U", 6);
    if(with_multisets) std::cout << pad_left("k_A", 6) << pad_left("k_B", 6);
    std::cout << std::endl;

    size_t rows = std::min(universe.size(), TABLE_LIMIT);
    for(size_t i = 0; i < rows; i++){
        std::cout << pad_left(std::to_string(i), 8) << "   " << pad_right(carrier->get_code(i), code_width)
                  << pad_left(std::to_string(universe.get_count(i)), 6);
        if(with_multisets){
            std::cout << pad_left(a_ready ? std::to_string(a.get_count(i)) : "-", 6)
                      << pad_left(b_ready ? std::to_string(b.get_count(i)) : "-", 6);
        }
        std::cout << std::endl;
    }
    if(universe.size() == 0){
        std::cout << "  (строк нет: при n = 0 носитель пуст)" << std::endl;
    }
    else if(rows < universe.size()){
        std::cout << "  ... показаны первые " << rows << " строк из " << universe.size() << std::endl;
    }

    std::cout << "Мощности: |U| = " << universe.cardinality();
    if(with_multisets && a_ready) std::cout << ", |A| = " << a.cardinality();
    if(with_multisets && b_ready) std::cout << ", |B| = " << b.cardinality();
    std::cout << std::endl;
}

void Menu::show_operations() const{
    print_header("Операции над A и B");
    std::cout << "U = " << format_multiset(universe) << ", |U| = " << universe.cardinality() << std::endl;
    std::cout << "A = " << format_multiset(a) << ", |A| = " << a.cardinality() << std::endl;
    std::cout << "B = " << format_multiset(b) << ", |B| = " << b.cardinality() << std::endl;

    std::vector<Operation> operations = compute_operations();
    for(size_t i = 0; i < operations.size(); i++){
        if(i == 0) std::cout << std::endl << "Теоретико-множественные операции:" << std::endl;
        if(i == 7) std::cout << std::endl << "Арифметические операции:" << std::endl;
        const Operation& op = operations[i];
        std::cout << "  " << pad_right(op.title, 28) << pad_right(op.notation, 6) << " = "
                  << format_multiset(op.result) << ", мощность " << op.result.cardinality() << std::endl;
    }
}

void Menu::show_operations_table() const{
    print_header("Кратности элементов в результатах операций");
    std::vector<Operation> operations = compute_operations();
    std::vector<std::string> titles = {"U", "A", "B"};
    std::vector<const Multiset*> columns = {&universe, &a, &b};
    for(size_t i = 0; i < operations.size(); i++){
        std::string title = operations[i].notation;
        title.erase(std::remove(title.begin(), title.end(), ' '), title.end());   // A ∪ B -> A∪B
        titles.push_back(title);
        columns.push_back(&operations[i].result);
    }

    size_t code_width = std::max<size_t>(carrier->get_depth(), 3);
    std::cout << pad_right("Код", code_width);
    for(size_t j = 0; j < titles.size(); j++) std::cout << pad_left(titles[j], 6);
    std::cout << std::endl;

    size_t rows = std::min(universe.size(), TABLE_LIMIT);
    for(size_t i = 0; i < rows; i++){
        std::cout << pad_right(carrier->get_code(i), code_width);
        for(size_t j = 0; j < columns.size(); j++){
            std::cout << pad_left(std::to_string(columns[j]->get_count(i)), 6);
        }
        std::cout << std::endl;
    }
    if(universe.size() == 0){
        std::cout << "  (строк нет: при n = 0 носитель пуст)" << std::endl;
    }
    else if(rows < universe.size()){
        std::cout << "  ... показаны первые " << rows << " строк из " << universe.size() << std::endl;
    }

    std::cout << pad_right("|·|", code_width);
    for(size_t j = 0; j < columns.size(); j++){
        std::cout << pad_left(std::to_string(columns[j]->cardinality()), 6);
    }
    std::cout << std::endl;
}
