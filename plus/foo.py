

OPPOSITE = {
    0: 0,
    1: 2,
    2: 1,
    3: 4,
    4: 3,
}

def rotate(p):
    return [p[1], p[2], p[3], p[0]]

def flip(p):
    # TODO flip for asymmetric
    return [p[3], p[2], p[1], p[0]]

def canonical(p):
    f = flip(p)
    transformations = [p, f]
    for i in range(3):
        p = rotate(p)
        f = rotate(f)
        transformations.append(p)
        transformations.append(f)
    return min(transformations)


import json

solutions = []
with open("foo.jl", "r") as file:
    for line in file:
        solution = json.loads(line)
        solutions.append(solution)

from collections import Counter

counter = Counter()
for solution in solutions:
    ps = tuple(sorted(tuple(canonical(p)) for row in solution for p in row))
    counter[ps] += 1


from svg import Buffer
import numpy as np
import io
import tempfile
import subprocess

from tqdm import tqdm

for i, solution in tqdm(enumerate(solutions), total=len(solutions)):

    matrix = np.array(solution)
    buffer = Buffer()
    buffer.jigsaw(matrix)
    
    with tempfile.NamedTemporaryFile("w") as file:
        buffer.save(file)
        
        path = r"C:\Program Files\Inkscape\inkscape.exe"
        subprocess.run([path, "-z", "-f", file.name, "-w", "512", "-j", "-e", f"foo.{i:03d}.png"])
    



q = [
[3, 0, 0, 4],
[3, 0, 4, 2],
[4, 0, 4, 4],
[4, 0, 3, 1],
[0, 0, 3, 1],
[3, 3, 0, 4],
[0, 1, 4, 3],
[3, 3, 0, 0],
[2, 2, 4, 2],
[0, 2, 1, 1],
[3, 3, 0, 2],
[4, 4, 4, 3],
[3, 0, 3, 1],
[2, 1, 4, 4],
[0, 2, 1, 4],
[1, 1, 0, 1],
[2, 4, 2, 3],
[4, 2, 1, 1],
[1, 3, 3, 4],
[0, 3, 2, 2],
[2, 2, 0, 0],
[1, 4, 1, 0],
[1, 2, 2, 0],
[2, 3, 2, 0],
[0, 1, 1, 0],
]
q = tuple(sorted(tuple(canonical(p)) for p in q))