
import numpy as np


def expand_edge_types(edge_types, opposite, flip):
    """Add missing edge types.

    Make sure that transformations (i.e. opposite and flip) produce edge types
    that are in the set.

    Parameters
    ----------
    edge_types: int, N
        ...
    opposite: int, K
        ...
    flip: int, K
        ...

    """

    _, = edge_types.shape

    # 4 possible transformations
    edge_types = np.concatenate([
        edge_types,
        opposite[edge_types],
        flip[edge_types],
        opposite[flip[edge_types]],
    ])

    # Deduplicate
    edge_types = np.unique(edge_types)
    return edge_types


def rotate_piece(pieces, k=1):
    """Rotate piece by k times 90°, counter-clockwise."""

    k = k % 4
    if k == 1:
        return rotate_piece_90(pieces)
    if k == 2:
        return rotate_piece_180(pieces)
    if k == 3:
        return rotate_piece_270(pieces)
    return pieces


def rotate_piece_90(pieces):
    """Rotate piece by 90°, counter-clockwise."""

    return pieces[..., (3, 0, 1, 2)]


def rotate_piece_180(pieces):
    """Rotate piece by 180°, counter-clockwise."""

    return pieces[..., (2, 3, 0, 1)]


def rotate_piece_270(pieces):
    """Rotate piece by 270°, counter-clockwise."""

    return pieces[..., (1, 2, 3, 0)]


def flip_piece(pieces, flip):
    """Flip piece diagonally."""

    return flip[pieces[..., (3, 2, 1, 0)]]


def flip_piece_h(pieces, flip):
    """Flip piece horizontally."""

    return flip[pieces[..., (2, 1, 0, 3)]]


def flip_piece_v(pieces, flip):
    """Flip piece vertically."""

    return flip[pieces[..., (0, 3, 2, 1)]]


def expand_pieces(pieces, flip, *, return_mapping=False):

    # Flatten array of pieces
    pieces = pieces.reshape(-1, 4)
    N, _ = pieces.shape

    # Instanciate all transformations
    a = pieces
    b = rotate_piece_90(a)
    c = rotate_piece_90(b)
    d = rotate_piece_90(c)
    e = flip_piece(pieces, flip)
    f = rotate_piece_90(e)
    g = rotate_piece_90(f)
    h = rotate_piece_90(g)
    pieces = np.concatenate([a, b, c, d, e, f, g, h], axis=0)

    # Deduplicate pieces
    if return_mapping:
        pieces, indices = np.unique(pieces, return_index=True, axis=0)
        indices %= N
        return pieces, indices
    else:
        pieces = np.unique(pieces, axis=0)
        return pieces


def _smallest(x):
    *S, T, D = x.shape
    x = x.reshape(-1, T, D)
    N, _, _ = x.shape
    y = np.empty((N, D), dtype=x.dtype)
    for i in range(N):
        indices = np.lexsort(x[i].T)
        y[i] = x[i, indices[0]]
    y = y.reshape(*S, D)
    return y


def canonize_piece(pieces, flip):

    # 8 possible transformations
    a = pieces
    b = rotate_piece_90(a)
    c = rotate_piece_90(b)
    d = rotate_piece_90(c)
    e = flip_piece(a, flip)
    f = rotate_piece_90(e)
    g = rotate_piece_90(f)
    h = rotate_piece_90(g)
    x = np.stack((a, b, c, d, e, f, g, h), axis=-2)
    
    # Select lexicographically smallest transformation
    y = _smallest(x)
    return y


def build_random_grid(H, W, K, *, generator=None, border=0):

    # Accept both an integer and a list of edge types
    if np.isscalar(K):
        edge_types = np.arange(K)
    else:
        edge_types = K
        K, = edge_types.shape

    # Generate random indices
    generator = np.random.default_rng(generator)
    horizontal_edges = generator.integers(1, K, size=(H, W + 1), dtype=np.uint8)
    vertical_edges = generator.integers(1, K, size=(H + 1, W), dtype=np.uint8)

    # Map to edge types
    horizontal_edges = edge_types[horizontal_edges]
    vertical_edges = edge_types[vertical_edges]

    # If requested, set border to a specific type
    if border is not None:
        horizontal_edges[:, 0] = border
        horizontal_edges[:, -1] = border
        vertical_edges[0, :] = border
        vertical_edges[-1, :] = border

    return horizontal_edges, vertical_edges


# TODO vectorize grid methods?


def grid_to_pieces(horizontal_edges, vertical_edges, opposite):
    """Convert grid to pieces."""

    H, _ = horizontal_edges.shape
    _, W = vertical_edges.shape
    assert horizontal_edges.shape == (H, W + 1)
    assert vertical_edges.shape == (H + 1, W)

    pieces = np.empty((H, W, 4), dtype=np.uint8)
    pieces[:, :, 0] = opposite[horizontal_edges[:, 1:]]
    pieces[:, :, 1] = opposite[vertical_edges[:-1, :]]
    pieces[:, :, 2] = horizontal_edges[:, :-1]
    pieces[:, :, 3] = vertical_edges[1:, :]

    return pieces


def pieces_to_grid(pieces, opposite):
    """Convert pieces to grid."""

    H, W, D = pieces.shape
    assert D == 4
    assert (pieces[:, :-1, 0] == opposite[pieces[:, 1:, 2]]).all()
    assert (pieces[:-1, :, 3] == opposite[pieces[1:, :, 1]]).all()

    horizontal_edges = np.empty((H, W + 1), dtype=np.uint8)
    horizontal_edges[:, 0] = pieces[:, 0, 2]
    horizontal_edges[:, 1:] = opposite[pieces[:, :, 0]]

    vertical_edges = np.empty((H + 1, W), dtype=np.uint8)
    vertical_edges[0, :] = opposite[pieces[0, :, 1]]
    vertical_edges[1:, :] = pieces[:, :, 3]

    return horizontal_edges, vertical_edges


def rotate_grid(horizontal_edges, vertical_edges, opposite, k=1):
    """Rotate grid by k times 90°, counter-clockwise."""

    k = k % 4
    if k == 1:
        return rotate_grid_90(horizontal_edges, vertical_edges, opposite)
    if k == 2:
        return rotate_grid_180(horizontal_edges, vertical_edges, opposite)
    if k == 3:
        return rotate_grid_270(horizontal_edges, vertical_edges, opposite)
    return horizontal_edges, vertical_edges


def rotate_grid_90(horizontal_edges, vertical_edges, opposite):
    """Rotate grid by 90°, counter-clockwise."""

    h = opposite[np.rot90(vertical_edges)]
    v = np.rot90(horizontal_edges)
    return h, v


def rotate_grid_180(horizontal_edges, vertical_edges, opposite):
    """Rotate grid by 180°, counter-clockwise."""

    h = opposite[np.rot90(horizontal_edges, k=2)]
    v = opposite[np.rot90(vertical_edges, k=2)]
    return h, v


def rotate_grid_270(horizontal_edges, vertical_edges, opposite):
    """Rotate grid by 270°, counter-clockwise."""

    h = np.rot90(vertical_edges, k=3)
    v = opposite[np.rot90(horizontal_edges, k=3)]
    return h, v


def flip_grid(horizontal_edges, vertical_edges, opposite, flip):
    """Flip grid diagonally."""

    h = opposite[flip[vertical_edges]].T
    v = opposite[flip[horizontal_edges]].T
    return h, v


def flip_grid_h(horizontal_edges, vertical_edges, opposite, flip):
    """Flip grid horizontally."""

    h = flip[opposite[horizontal_edges[:, ::-1]]]
    v = flip[vertical_edges[:, ::-1]]
    return h, v


def flip_grid_v(horizontal_edges, vertical_edges, opposite, flip):
    """Flip grid vertically."""

    h = flip[horizontal_edges[::-1, :]]
    v = flip[opposite[vertical_edges[::-1, :]]]
    return h, v


def canonize_grid(horizontal_edges, vertical_edges, opposite, flip):

    H, _ = horizontal_edges.shape
    _, W = vertical_edges.shape
    assert horizontal_edges.shape == (H, W + 1)
    assert vertical_edges.shape == (H + 1, W)

    a = horizontal_edges, vertical_edges

    # 8 possible transformations for square grids
    if H == W:
        b = rotate_grid_90(*a, opposite)
        c = rotate_grid_90(*b, opposite)
        d = rotate_grid_90(*c, opposite)
        e = flip_grid(*a, opposite, flip)
        f = rotate_grid_90(*e, opposite)
        g = rotate_grid_90(*f, opposite)
        h = rotate_grid_90(*g, opposite)
        ts = (a, b, c, d, e, f, g, h)

    else:

        # Rectangular grids are always tall in canonical form (i.e. H > W)
        if H < W:
            a = rotate_grid_cw(*a, opposite)
            H, W = W, H

        # 4 possible transformations for rectangular grids
        b = rotate_grid_180(*a, opposite)
        c = flip_grid_h(*a, opposite, flip)
        d = rotate_grid_180(*c, opposite)
        ts = (a, b, c, d)

    # Flatten and pack as a single matrix
    m = np.stack([np.concatenate([t[0].reshape(-1), t[1].reshape(-1)]) for t in ts])

    # Sort and take the smallest one
    indices = np.lexsort(m.T)
    t = ts[indices[0]]
    return t
