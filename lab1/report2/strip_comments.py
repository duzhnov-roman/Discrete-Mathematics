"""Копирует исходники lab1 в listings/ без комментариев (исходники не меняются).
C++: // и /* */ вне строковых и символьных литералов; CMake: # вне строк.
Строки, где был только комментарий, удаляются; подряд идущие пустые строки
сводятся к одной. Запуск из папки report2: python strip_comments.py"""
import os, re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, '..')
FILES = ['CMakeLists.txt', 'src/main.cpp',
         'src/ui/Input.hpp', 'src/ui/Input.cpp', 'src/ui/Menu.hpp', 'src/ui/Menu.cpp',
         'src/core/GrayCode.hpp', 'src/core/GrayCode.cpp',
         'src/core/Multiset.hpp', 'src/core/Multiset.cpp',
         'src/core/Random.hpp', 'src/core/Random.cpp',
         'src/core/Universe.hpp', 'src/core/Universe.cpp',
         'tools/bench.cpp']
GONE = '\0'   # метка строки, в которой остался только комментарий


def strip_cpp(text):
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c in '"\'':
            j = i + 1
            while text[j] != c:
                j += 2 if text[j] == '\\' else 1
            out.append(text[i:j + 1]); i = j + 1
        elif text.startswith('//', i):
            while i < n and text[i] != '\n':
                i += 1
            out.append(GONE)
        elif text.startswith('/*', i):
            j = text.index('*/', i + 2)
            out.append(GONE + ('\n' + GONE) * text.count('\n', i, j)); i = j + 2
        else:
            out.append(c); i += 1
    return ''.join(out)


def strip_cmake(text):
    res = []
    for line in text.split('\n'):
        inside = False
        for k, ch in enumerate(line):
            if ch == '"':
                inside = not inside
            elif ch == '#' and not inside:
                line = line[:k] + GONE
                break
        res.append(line)
    return '\n'.join(res)


def tidy(text):
    lines = []
    for line in text.split('\n'):
        had = GONE in line
        line = line.replace(GONE, '').rstrip()
        if had and not line:
            continue
        lines.append(line)
    res = []
    for line in lines:
        if not line and (not res or not res[-1]):
            continue
        res.append(line)
    while res and not res[-1]:
        res.pop()
    return '\n'.join(res) + '\n'


for name in FILES:
    with open(os.path.join(ROOT, name), encoding='utf-8') as f:
        text = f.read().replace('\r\n', '\n')
    clean = tidy(strip_cmake(text) if name.endswith('.txt') else strip_cpp(text))
    path = os.path.join(HERE, 'listings', name)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(clean)
    print(name, text.count('\n'), '->', clean.count('\n'))
