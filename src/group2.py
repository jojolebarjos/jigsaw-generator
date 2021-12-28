
import json

import numpy as np

from PIL import Image


jigsaws = []
with open("jigsaw.jl", "r") as file:
    for line in file:
        jigsaw = json.loads(line)
        jigsaws.append(jigsaw)


X = np.array(jigsaws, dtype=int)
X = X.reshape(-1, 4, 4, 4)

BITS = 3
MASK = (1 << BITS) - 1
NUM = 1 << BITS
canonical = np.zeros(NUM ** 4, dtype=int)
for p in range(NUM ** 4):
    canonical[p] = p
    q = p
    for i in range(4):
        q = (q >> BITS) | ((q & MASK) << (3 * BITS))
        if q < p:
            break
    canonical[p] = canonical[q]
#assert len(set(canonical)) == 70



Y = X[:, :, :, 0] | X[:, :, :, 1] << 2 | X[:, :, :, 2] << 4 | X[:, :, :, 3] << 6

_, height, width, _ = X.shape
Z = canonical[Y].reshape(-1, height * width)
Z = np.sort(Z)


index = np.random.randint(Z.shape[0])

jigsaw = X[index] + 1


colors = np.array([
    [255, 255, 255],
    [128, 128, 128],
    [255,   0,   0],
    [255,   0,   0],
    [  0,   0, 255],
    [  0,   0, 255],
    [  0, 255,   0],
    [  0, 255,   0],
], dtype=np.uint8)


symmetric_patterns = np.zeros((4, 9, 9), dtype=int)
symmetric_patterns[0, 1:-1, -1] = 1
symmetric_patterns[1, 0, 1:-1] = 1
symmetric_patterns[2, 1:-1, 0] = 1
symmetric_patterns[3, -1, 1:-1] = 1

male_patterns = symmetric_patterns

female_patterns = np.zeros((4, 9, 9), dtype=int)
female_patterns[0, 1:-1, -1] = 1
female_patterns[0, 3:-3, -2] = 1
female_patterns[0, 4, -3] = 1
female_patterns[1, 0, 1:-1] = 1
female_patterns[1, 1, 3:-3] = 1
female_patterns[1, 2, 4] = 1
female_patterns[2, 1:-1, 0] = 1
female_patterns[2, 3:-3, 1] = 1
female_patterns[2, 4, 2] = 1
female_patterns[3, -1, 1:-1] = 1
female_patterns[3, -2, 3:-3] = 1
female_patterns[3, -3, 4] = 1

patterns = np.stack([
    symmetric_patterns,
    female_patterns,
    male_patterns,
    female_patterns,
    male_patterns,
], axis=0)

height, width, _ = jigsaw.shape
matrix = np.zeros((height * 9, width * 9), dtype=int)
for y in range(height):
    for x in range(width):
        chunk = np.zeros((9, 9), dtype=int)
        for i in range(4):
            k = jigsaw[y, x, i]
            chunk[:] += patterns[k - 1, i] * k
        matrix[y*9:(y+1)*9, x*9:(x+1)*9] = chunk

image = colors[matrix]

image = Image.fromarray(image)
image.save(f"jigsaw.png")
