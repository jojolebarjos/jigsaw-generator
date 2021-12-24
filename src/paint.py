
import json

import numpy as np

from PIL import Image


jigsaws = []
with open("jigsaw.jl", "r") as file:
    for line in file:
        jigsaw = json.loads(line)
        jigsaws.append(jigsaw)


jigsaw = jigsaws[0]

jigsaw = np.array(jigsaw, dtype=int)

num_types = jigsaw.max() + 1
height, width, _ = jigsaw.shape

jigsaw += 1

colors = np.array([
    [255, 255, 255],
    [255,   0,   0],
    [  0, 255,   0],
    [  0,   0, 255],
    [255, 255,   0],
    [  0, 255, 255],
    [255,   0, 255],
    [128,   0,   0],
    [  0, 128,   0],
    [  0,   0, 128],
    [128, 128,   0],
    [  0, 128, 128],
    [128,   0, 128],
], dtype=np.uint8)


patterns = np.zeros((4, 9, 9), dtype=int)
patterns[0, 1:-1, -1] = 1
patterns[1, 0, 1:-1] = 1
patterns[2, 1:-1, 0] = 1
patterns[3, -1, 1:-1] = 1

matrix = (
    np.kron(jigsaw[:, :, 0], patterns[0]) +
    np.kron(jigsaw[:, :, 1], patterns[1]) +
    np.kron(jigsaw[:, :, 2], patterns[2]) +
    np.kron(jigsaw[:, :, 3], patterns[3])
)

image = colors[matrix]

image = Image.fromarray(image)
image.save("jigsaw.png")
