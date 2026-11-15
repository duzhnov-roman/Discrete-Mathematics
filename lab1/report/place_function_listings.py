"""Размещает код каждой функции сразу после её описания в отчёте."""
from pathlib import Path
import re
import subprocess
import sys

HERE = Path(__file__).resolve().parent


def mask_literals(text):
    return re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)


def closing(text, start, left, right):
    depth = 0
    for pos in range(start, len(text)):
        if text[pos] == left:
            depth += 1
        elif text[pos] == right:
            depth -= 1
            if depth == 0:
                return pos + 1
    raise ValueError('Незакрытая скобка')


def function_code(path, name):
    text = (HERE / 'listings' / path).read_text(encoding='utf-8-sig')
    masked = mask_literals(text)
    for match in re.finditer(r'(?<![\w:])' + re.escape(name) + r'\s*\(', masked):
        signature_end = closing(masked, match.end() - 1, '(', ')')
        body = masked.find('{', signature_end)
        semicolon = masked.find(';', signature_end)
        if body < 0 or (semicolon >= 0 and semicolon < body):
            continue
        start = text.rfind('\n', 0, match.start()) + 1
        prefix = text[start:match.start()]
        if '=' in prefix or 'return ' in prefix:
            continue
        end = closing(masked, body, '{', '}')
        return text[start:end].strip() + '\n'
    raise ValueError(f'Не найдена функция {name} в {path}')


def tex_group(text, start):
    start = re.search(r'\S', text[start:]).start() + start
    assert text[start] == '{'
    return closing(text, start, '{', '}')


def source_for(name):
    if name in ['changed_bit', 'generate_gray_code', 'code_to_string'] or name.startswith('CodeSet::'):
        return 'src/core/GrayCode.cpp', name
    if name in ['generator', 'make_seed', 'seed_random', 'random_below']:
        return 'src/core/Random.cpp', name
    if name.startswith('Multiset::') or name in ['random_submultiset', 'print_multiset', 'operator<<']:
        return 'src/core/Multiset.cpp', name
    if name == 'uniform_universe':
        return 'src/core/Universe.cpp', name
    if name in ['stdin_is_console', 'read_text', 'parse_number', 'is_code',
                'report_bad_number', 'read_number', 'read_number_or_empty', 'read_letter']:
        return 'src/ui/Input.cpp', name
    if name.startswith('Menu::') or name in ['text_width', 'pad_right', 'pad_left', 'format_multiset', 'compact']:
        return 'src/ui/Menu.cpp', name
    if name == 'seconds':
        return 'tools/bench.cpp', name
    cls, ctor = name.split('::')
    if cls in ['GrayCodeError', 'InvalidDepthError', 'CodeIndexError']:
        return 'src/core/GrayCode.hpp', ctor
    if cls == 'InvalidMultiplicityError':
        return 'src/core/Universe.hpp', ctor
    if cls == 'InputClosedError':
        return 'src/ui/Input.hpp', ctor
    return 'src/core/Multiset.hpp', ctor


def main():
    subprocess.run([sys.executable, str(HERE / 'strip_comments.py')], check=True)
    folder = HERE / 'functions'
    folder.mkdir(exist_ok=True)
    file = HERE / 'implementation.tex'
    text = file.read_text(encoding='utf-8')
    text = re.sub(r'\n\\functioncode\{[^\n]*\}\{[^\n]*\}\n', '\n', text)
    inserts = []
    names = []
    for number, match in enumerate(re.finditer(r'\\fn\{([^}]+)\}', text), 1):
        name = match[1]
        source, symbol = source_for(name)
        snippet = function_code(source, symbol)
        if name == 'stdin_is_console':
            snippet = ('#ifdef _WIN32\n' + snippet + '#else\n'
                       + 'static bool stdin_is_console(){ return isatty(0) != 0; }\n#endif\n')
        filename = f'{number:02}.cpp'
        assert '//' not in snippet and '/*' not in snippet
        (folder / filename).write_text(snippet, encoding='utf-8')
        end = tex_group(text, match.end())
        end = tex_group(text, end)
        inserts.append((end, '\n\\functioncode{' + name + '}{' + filename + '}\n'))
        names.append(name)
    for pos, addition in reversed(inserts):
        text = text[:pos] + addition + text[pos:]
    (folder / 'main.cpp').write_text(function_code('src/main.cpp', 'main'), encoding='utf-8')
    (folder / 'bench_main.cpp').write_text(function_code('tools/bench.cpp', 'main'), encoding='utf-8')
    (folder / 'seconds.cpp').write_text(function_code('tools/bench.cpp', 'seconds'), encoding='utf-8')
    sentence = 'При входе в \\code{main} происходит инициализация параметров.'
    text = text.replace(sentence, sentence + '\n\\functioncode{main}{main.cpp}\n')
    bench_desc = 'Программа \\code{bench} измеряет создание универсума $U$.'
    assert bench_desc in text
    text = text.replace(bench_desc, bench_desc + '\n\\functioncode{main (tools/bench.cpp)}{bench_main.cpp}\n'
                        + '\n\\functioncode{seconds}{seconds.cpp}\n')
    file.write_text(re.sub(r'\n{3,}', '\n\n', text), encoding='utf-8')
    assert names
    assert len(re.findall(r'\\functioncode\{', text)) == len(names) + 3
    used = {f'{number:02}.cpp' for number in range(1, len(names) + 1)} | {'main.cpp', 'bench_main.cpp', 'seconds.cpp'}
    for path in folder.glob('*.cpp'):
        if path.name not in used:
            assert path.resolve().parent == folder.resolve()
            path.unlink()
    print('Листинги размещены:', len(names), 'функций и конструкторов, main и main программы замера.')


if __name__ == '__main__':
    main()
