"""Рисунки раздела 3: запускает lab1 на сценариях из scenarios/ с фиксированным зерном
и отрисовывает протоколы в PNG в виде окна консоли Windows (Consolas, недостающие
глифы — из Segoe UI Symbol, как в самой консоли).
Запуск из папки report: python make_figures.py (нужны Pillow и fontTools, собранный ../lab1.exe)."""
import os, re, subprocess
from fontTools.ttLib import TTFont
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
PROGRAM = os.path.join(HERE, '..', 'lab1.exe')
# сценарий -> зерно генератора
SEEDS = {'s1_auto': 1, 's2_manual': 2, 's3_bounds': 3, 's4_errors': 4,
         's5_zero': 5, 's6_recreate_eof': 6, 's7_large': 7}
SIZE = 17
MAIN = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', SIZE)
FALL = ImageFont.truetype('C:/Windows/Fonts/seguisym.ttf', SIZE + 1)
CMAP = TTFont('C:/Windows/Fonts/consola.ttf').getBestCmap()
CW = MAIN.getlength('M'); LH = 21; PAD = 12; WRAP = 130
BG = (12, 12, 12); FG = (204, 204, 204)


def transcript(scenario):
    with open(os.path.join(HERE, 'scenarios', scenario + '.txt'), 'rb') as stdin:
        out = subprocess.run([PROGRAM, '--seed', str(SEEDS[scenario])], stdin=stdin,
                             stdout=subprocess.PIPE, check=True).stdout
    return out.decode('utf-8').replace('\r\n', '\n').split('\n')


def blocks(lines):
    """действия пользователя: от ввода команды «> » после пустой строки до следующей пустой строки"""
    starts = [i for i in range(1, len(lines)) if lines[i].startswith('> ') and lines[i - 1] == '']
    res = []
    for s in starts:
        end = s + 1
        while end < len(lines) and lines[end] != '':
            end += 1
        res.append((s, end))
    return res


def render(lines, path):
    rows = []
    for l in lines:
        while len(l) > WRAP:
            rows.append(l[:WRAP]); l = l[WRAP:]
        rows.append(l)
    while rows and rows[-1] == '':
        rows.pop()
    cols = max(len(r) for r in rows)
    w = int(PAD * 2 + CW * max(cols, 40)); h = PAD * 2 + LH * len(rows)
    img = Image.new('RGB', (w, h), BG)
    d = ImageDraw.Draw(img)
    for y, r in enumerate(rows):
        for x, ch in enumerate(r):
            px, py = PAD + x * CW, PAD + y * LH
            if ord(ch) in CMAP:
                d.text((px, py), ch, font=MAIN, fill=FG)
            else:
                # как консоль Windows: недостающий глиф берётся из Segoe UI Symbol и центрируется по ячейке
                l, t, r, b = d.textbbox((0, 0), ch, font=FALL)
                ref = d.textbbox((0, 0), 'n', font=MAIN)
                cy = (ref[1] + ref[3]) / 2
                d.text((px + (CW - (r - l)) / 2 - l, py + cy - (t + b) / 2), ch, font=FALL, fill=FG)
    img.save(path)
    print(os.path.basename(path), len(rows), 'строк')


def figure(name, scenario, first, last, cut_from=None, cut_to=None):
    """рисунок из действий с номерами first..last; cut_from/cut_to обрезают его по строкам"""
    lines = transcript(scenario)
    bl = blocks(lines)
    part = []
    for k in range(first, last + 1):
        if k > first:
            part.append('')
        block = lines[bl[k][0]:bl[k][1]]
        while block and block[-1] == '':
            block.pop()
        part += block
    if cut_from:
        part = part[next(i for i, l in enumerate(part) if re.search(cut_from, l)):]
    if cut_to:
        part = part[:next(i for i, l in enumerate(part) if re.search(cut_to, l))]
    render(part, os.path.join(HERE, name + '.png'))


def head(name, scenario, upto_regex):
    lines = transcript(scenario)
    end = next(i for i, l in enumerate(lines) if re.search(upto_regex, l))
    render(lines[:end + 1], os.path.join(HERE, name + '.png'))


# сценарий 1: случайная кратность, автоматическое заполнение (зерно 1)
head('fig_s1_start', 's1_auto', r'^> h$')
figure('fig_s1_help', 's1_auto', 0, 0)
figure('fig_s1_universe', 's1_auto', 1, 1)
figure('fig_s1_fill', 's1_auto', 2, 3)
figure('fig_s1_table', 's1_auto', 4, 4)
figure('fig_s1_ops', 's1_auto', 5, 5)
figure('fig_s1_optable', 's1_auto', 6, 6)
# сценарий 2: ручной ввод (зерно 2)
figure('fig_s2_universe', 's2_manual', 0, 0)
figure('fig_s2_fill_a', 's2_manual', 1, 1)
figure('fig_s2_fill_b', 's2_manual', 2, 3)
figure('fig_s2_ops', 's2_manual', 4, 4)
figure('fig_s2_optable', 's2_manual', 5, 5)
# сценарий 3: граничные мощности, A = U, B = пусто (зерно 3)
figure('fig_s3_universe', 's3_bounds', 0, 0)
figure('fig_s3_fill', 's3_bounds', 1, 2)
figure('fig_s3_ops', 's3_bounds', 4, 4)
figure('fig_s3_optable', 's3_bounds', 5, 5)
# сценарий 4: некорректный ввод (зерно 4)
figure('fig_s4_order', 's4_errors', 0, 1)
figure('fig_s4_menu', 's4_errors', 2, 2, cut_to=r'^n \[')
figure('fig_s4_universe', 's4_errors', 2, 2, cut_from=r'^n \[')
figure('fig_s4_rest', 's4_errors', 3, 4)
# сценарий 5: n = 0 (зерно 5)
figure('fig_s5_universe', 's5_zero', 0, 0)
figure('fig_s5_fill', 's5_zero', 1, 2)
figure('fig_s5_table', 's5_zero', 3, 3)
figure('fig_s5_ops', 's5_zero', 4, 4)
figure('fig_s5_optable', 's5_zero', 5, 5)
# сценарий 6: пересоздание и конец ввода (зерно 6)
figure('fig_s6_before', 's6_recreate_eof', 1, 2)
figure('fig_s6_recreate', 's6_recreate_eof', 3, 3)
figure('fig_s6_eof', 's6_recreate_eof', 4, 4)
# сценарий 7: n = 19 (зерно 7)
figure('fig_s7_universe', 's7_large', 0, 0)
figure('fig_s7_fill', 's7_large', 1, 1)
figure('fig_s7_table', 's7_large', 2, 2)
