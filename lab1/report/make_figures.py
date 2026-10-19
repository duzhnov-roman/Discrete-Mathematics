"""Запускает build/lab1.exe на сценариях из scenarios/ с фиксированным зерном,
сохраняет протоколы в out/ и рисует фрагменты в fig/*.png в виде окна консоли.
Также пишет fig/widths.tex: ширина каждого рисунка в долях \\textwidth,
чтобы шрифт на всех рисунках был одного размера.
Запуск из папки report2: python make_figures.py"""
import os, re, subprocess
from fontTools.ttLib import TTFont
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
PROGRAM = os.path.join(HERE, 'build', 'lab1.exe')
SEEDS = {'s1_correct': 1, 's2_input': 2, 's3_manual': 3, 's4_bounds': 4,
         's5_zero_eof': 5, 's6_max': 6}
SIZE = 30
MAIN = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', SIZE)
FALL = ImageFont.truetype('C:/Windows/Fonts/seguisym.ttf', SIZE)
CMAP = TTFont('C:/Windows/Fonts/consola.ttf').getBestCmap()
CW = MAIN.getlength('M'); LH = int(SIZE * 1.25); PAD = 18
WRAP = 112          # столбцов в строке, длиннее — перенос
FULL = 122          # столбцов на полную ширину текста
BG = (12, 12, 12); FG = (220, 220, 220)
widths = {}


def transcript(scenario):
    with open(os.path.join(HERE, 'scenarios', scenario + '.txt'), 'rb') as stdin:
        out = subprocess.run([PROGRAM, '--seed', str(SEEDS[scenario])], stdin=stdin,
                             stdout=subprocess.PIPE, check=True).stdout
    text = out.decode('utf-8').replace('\r\n', '\n')
    with open(os.path.join(HERE, 'out', scenario + '.txt'), 'w', encoding='utf-8') as f:
        f.write(text)
    return text.split('\n')


def blocks(lines):
    """действие: от списка команд до пустой строки перед следующим списком"""
    starts = [i for i in range(len(lines)) if lines[i].startswith('u - универсум')]
    res = []
    for k, s in enumerate(starts):
        end = starts[k + 1] - 1 if k + 1 < len(starts) else len(lines)
        res.append((s, end))
    return res


def render(lines, name):
    rows = []
    for l in lines:
        while len(l) > WRAP:
            rows.append(l[:WRAP]); l = l[WRAP:]
        rows.append(l)
    while rows and rows[-1] == '':
        rows.pop()
    cols = max(max(len(r) for r in rows), 50)
    w = int(PAD * 2 + CW * cols); h = PAD * 2 + LH * len(rows)
    img = Image.new('RGB', (w, h), BG)
    d = ImageDraw.Draw(img)
    ref = d.textbbox((0, 0), 'n', font=MAIN)
    cy = (ref[1] + ref[3]) / 2
    for y, r in enumerate(rows):
        for x, ch in enumerate(r):
            px, py = PAD + x * CW, PAD + y * LH
            if ord(ch) in CMAP:
                d.text((px, py), ch, font=MAIN, fill=FG)
            else:
                l, t, rr, b = d.textbbox((0, 0), ch, font=FALL)
                d.text((px + (CW - (rr - l)) / 2 - l, py + cy - (t + b) / 2), ch, font=FALL, fill=FG)
    img.save(os.path.join(HERE, 'fig', name + '.png'))
    widths[name] = min(1.0, (cols + 2 * PAD / CW) / FULL)
    print(name, len(rows), 'строк', cols, 'столбцов')


def figure(name, lines, first, last, cut_from=None, cut_to=None, head=False):
    bl = blocks(lines)
    part = lines[:bl[0][0]] if head else []
    for k in range(first, last + 1):
        if part and part[-1] != '':
            part.append('')
        part += lines[bl[k][0]:bl[k][1]]
    if cut_from:
        part = part[next(i for i, l in enumerate(part) if re.search(cut_from, l)):]
    if cut_to:
        part = part[:next(i for i, l in enumerate(part) if re.search(cut_to, l))]
    while part and part[0] == '':
        part.pop(0)
    render(part, name)


L = transcript('s1_correct')
figure('s1_help', L, 0, 0, head=True)
figure('s1_universe', L, 1, 1)
figure('s1_fill', L, 2, 3)
figure('s1_table', L, 4, 4)
figure('s1_ops', L, 5, 5)
figure('s1_matrix', L, 6, 7)

L = transcript('s2_input')
figure('s2_unavail', L, 0, 1, head=True)
figure('s2_menu', L, 2, 2, cut_to=r'^n \[')
figure('s2_params', L, 2, 2, cut_from=r'^n \[')
figure('s2_fill', L, 3, 7)

L = transcript('s3_manual')
figure('s3_setup', L, 0, 1, head=True)
figure('s3_errors', L, 2, 2)
figure('s3_table', L, 3, 3)
figure('s3_ok', L, 4, 7)

L = transcript('s4_bounds')
figure('s4_fill', L, 0, 2, head=True)
figure('s4_ops', L, 4, 4)
figure('s4_max', L, 6, 8)
figure('s4_ops2', L, 9, 10)

L = transcript('s5_zero_eof')
figure('s5_zero', L, 0, 3, head=True)
figure('s5_ops', L, 4, 4)
figure('s5_recreate', L, 6, 8)

L = transcript('s6_max')
figure('s6_universe', L, 0, 0, head=True)
figure('s6_fill', L, 1, 2)
figure('s6_table', L, 3, 3)
figure('s6_ops', L, 4, 4)
figure('s6_matrix', L, 5, 6)

with open(os.path.join(HERE, 'fig', 'widths.tex'), 'w', encoding='utf-8') as f:
    for name, w in widths.items():
        f.write('\\expandafter\\def\\csname fw@%s\\endcsname{%.3f}\n' % (name, w))
