
import json

import numpy as np

from PIL import Image

try:
    from IPython.lib.pretty import pprint
except ImportError:
    pprint = print


# Choose colors for each edge type (0 is None)
colors = np.array([
    [255, 255, 255],
    [128, 128, 128],
    [255,   0,   0],
    [255,   0,   0],
    [  0,   0, 255],
    [  0,   0, 255],
    [  0, 255,   0],
    [  0, 255,   0],
    # ...
], dtype=np.uint8)


# Generate 9x9 tiles
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
    female_patterns,
    male_patterns,
    # ...
], axis=0)

# Load solutions
# Note: we only have one solution per set, so we assume these are unique (e.g. `generate` reported a minimum of 4 solutions)
jigsaws = []
with open("jigsaw.jl", "r") as file:
    for line in file:
        jigsaw = json.loads(line)
        jigsaws.append(jigsaw)

# Wrap as array
HEIGHT, WIDTH = 4, 4
X = np.array(jigsaws, dtype=int)
X = X.reshape(-1, HEIGHT, WIDTH, 4)

# Select N random jigsaw
indices = np.arange(X.shape[0])
np.random.shuffle(indices)
N = 5
indices = indices[:N]

# Export them
for index in indices:

    # Print to console
    print(f"{index}:")
    pprint(X[index].tolist())

    # Convert to colors
    jigsaw = X[index] + 1
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

    # Export image
    image = Image.fromarray(image)
    image.save(f"jigsaw.{index}.png")
