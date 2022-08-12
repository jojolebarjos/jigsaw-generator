
import numpy as np

from numba import uint8, uintc
from numba.experimental import jitclass


spec = [
    ("C", uintc),
    ("N", uintc),
    ("H", uintc),
    ("W", uintc),
    ("count", uint8[::1]), # C
    ("canonical", uint8[::1]), # N
    ("accept", uint8[:, ::1]), # H*W x N
    ("horizontal", uint8[:, ::1]), # N x N
    ("vertical", uint8[:, ::1]), # N x N
    ("stack", uint8[::1]), # (H+1)*W
    ("depth", uint8),
    ("has", uint8),
]

@jitclass(spec)
class Solver:
    def __init__(self, C, N, H, W, count, canonical, accept, horizontal, vertical):
        self.C = C
        self.N = N
        self.H = H
        self.W = W
        self.count = count
        self.canonical = canonical
        self.accept = accept
        self.horizontal = horizontal
        self.vertical = vertical
        self.stack = np.zeros((H + 1) * W, dtype=np.uint8)
        self.depth = W
        self.has = 0

    def step(self):
        
        # Loop until a solution is found
        while True:

            # Check whether current piece matches
            if (
                self.has and
                self.count[self.canonical[self.stack[self.depth]]] and
                self.accept[self.depth - self.W, self.stack[self.depth]] and
                self.horizontal[self.stack[self.depth - 1], self.stack[self.depth]] and
                self.vertical[self.stack[self.depth - self.W], self.stack[self.depth]]
            ):

                # If board is full, we have a solution
                if self.depth == (self.H + 1) * self.W - 1:
                    self.has = 1
                    return 1

                # Otherwise, we need to go deeper
                self.count[self.canonical[self.stack[self.depth]]] -= 1
                self.depth += 1
                self.stack[self.depth] = 0
                continue

            has = 0

            # Check whether current depth is exhausted
            while self.stack[self.depth] == self.N - 1:

                # Search is complete
                if self.depth == self.W:
                    return 0

                # Backtrack one depth level
                self.depth -= 1
                self.count[self.canonical[self.stack[self.depth]]] += 1

            # Select next piece
            self.stack[self.depth] += 1


def iterate_solutions(C, N, H, W, count, canonical, accept, horizontal, vertical):

    assert count.shape == (C,)
    assert canonical.shape == (N,)
    assert accept.shape == (H, W, N)
    assert horizontal.shape == (N, N)
    assert vertical.shape == (N, N)

    accept = accept.reshape(H * W, N)

    solver = Solver(C, N, H, W, count, canonical, accept, horizontal, vertical)
    while solver.step():
        yield solver.stack[W:]
