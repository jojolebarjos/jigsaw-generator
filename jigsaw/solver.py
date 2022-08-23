
import numpy as np

from numba import uint8, uintc
from numba.experimental import jitclass

from .transform import canonize_piece, expand_pieces, pieces_to_grid


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
        self.stack = np.zeros(H * W, dtype=np.uint8)
        self.depth = 0
        self.has = 0

    def step(self):

        # Loop until a solution is found
        while True:

            # Check whether current piece matches
            if (
                not self.has and
                self.count[self.canonical[self.stack[self.depth]]] and
                self.accept[self.depth, self.stack[self.depth]] and
                (self.depth % self.W == 0 or self.horizontal[self.stack[self.depth - 1], self.stack[self.depth]]) and
                (self.depth < self.W or self.vertical[self.stack[self.depth - self.W], self.stack[self.depth]])
            ):

                # If board is full, we have a solution
                if self.depth == self.H * self.W - 1:
                    self.has = 1
                    return 1

                # Otherwise, we need to go deeper
                self.count[self.canonical[self.stack[self.depth]]] -= 1
                self.depth += 1
                self.stack[self.depth] = 0
                continue

            self.has = 0

            # Check whether current depth is exhausted
            while self.stack[self.depth] == self.N - 1:

                # Search is complete
                if self.depth == 0:
                    return 0

                # Backtrack one depth level
                self.depth -= 1
                self.count[self.canonical[self.stack[self.depth]]] += 1

            # Select next piece
            self.stack[self.depth] += 1


def iterate_solver(C, N, H, W, count, canonical, accept, horizontal, vertical):

    assert count.shape == (C,)
    assert canonical.shape == (N,)
    assert accept.shape == (H, W, N)
    assert horizontal.shape == (N, N)
    assert vertical.shape == (N, N)

    accept = accept.reshape(H * W, N)

    solver = Solver(C, N, H, W, count, canonical, accept, horizontal, vertical)
    while solver.step():
        yield solver.stack.reshape(H, W)


def prepare_solver_arguments(H, W, pieces, opposite, flip, constraints=None, *, use_proper_types=True):

    # Generate list of canonical pieces
    pieces = pieces.reshape(-1, 4)
    canonical_pieces = canonize_piece(pieces, flip)
    canonical_pieces, count = np.unique(canonical_pieces, return_counts=True, axis=0)
    C, _ = canonical_pieces.shape

    # Expand all transformations
    pieces, canonical = expand_pieces(canonical_pieces, flip, return_mapping=True)
    N, _ = pieces.shape

    # Create compatibility matrices
    horizontal = pieces[:, None, 0] == opposite[pieces[None, :, 2]]
    vertical = pieces[:, None, 3] == opposite[pieces[None, :, 1]]

    # If there is no constraint, acceptance matrix is trivial
    if constraints is None:
        horizontal_constraints = np.full((H, W + 1), -1)
        vertical_constraints = np.full((H + 1, W), -1)

    # Special flag for typical flat border
    elif constraints == "border":
        horizontal_constraints = np.full((H, W + 1), -1)
        vertical_constraints = np.full((H + 1, W), -1)
        horizontal_constraints[:, [0, -1]] = 0
        vertical_constraints[[0, -1], :] = 0

    # Unpack expected arrays
    else:
        horizontal_constraints, vertical_constraints = constraints
        assert horizontal_constraints.shape == (H, W + 1)
        assert vertical_constraints.shape == (H + 1, W)

    # Create acceptance matrix
    a = (horizontal_constraints[:, 1:, None] < 0) | (horizontal_constraints[:, 1:, None] == opposite[pieces[None, None, :, 0]])
    b = (vertical_constraints[:-1, :, None] < 0) | (vertical_constraints[:-1, :, None] == opposite[pieces[None, None, :, 1]])
    c = (horizontal_constraints[:, :-1, None] < 0) | (horizontal_constraints[:, :-1, None] == pieces[None, None, :, 2])
    d = (vertical_constraints[1:, :, None] < 0) | (vertical_constraints[1:, :, None] == pieces[None, None, :, 3])
    accept = a & b & c & d

    # Make sure we are using the expected types
    if use_proper_types:
        count = count.astype(np.uint8)
        canonical = canonical.astype(np.uint8)
        horizontal = horizontal.astype(np.uint8)
        vertical = vertical.astype(np.uint8)
        accept = accept.astype(np.uint8)

    return pieces, C, N, count, canonical, accept, horizontal, vertical


def iterate_solutions(H, W, pieces, opposite, flip, constraints=None):
    pieces, C, N, count, canonical, accept, horizontal, vertical = prepare_solver_arguments(H, W, pieces, opposite, flip, constraints)
    for stack in iterate_solver(C, N, H, W, count, canonical, accept, horizontal, vertical):
        indices = stack.reshape(H, W)
        grid = pieces_to_grid(pieces[indices], opposite)
        yield grid
