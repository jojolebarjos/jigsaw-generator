
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


def rotate_piece_cw(pieces):
    return pieces[..., (1, 2, 3, 0)]


def rotate_piece_ccw(pieces):
    return pieces[..., (3, 0, 1, 2)]


def flip_piece(pieces, flip):
    return flip[pieces[..., (3, 2, 1, 0)]]


def expand_pieces(pieces, flip, *, return_mapping=False):

    # Flatten array of pieces
    pieces = pieces.reshape(-1, 4)
    N, _ = pieces.shape

    # Instanciate all transformations
    a = pieces
    b = rotate_piece_cw(a)
    c = rotate_piece_cw(b)
    d = rotate_piece_cw(c)
    e = flip_piece(pieces, flip)
    f = rotate_piece_cw(e)
    g = rotate_piece_cw(f)
    h = rotate_piece_cw(g)
    pieces = np.concatenate([a, b, c, d, e, f, g, h], axis=0)

    # Deduplicate pieces
    if return_mapping:
        pieces, indices = np.unique(pieces, return_index=True, axis=0)
        indices %= N
        return pieces, indices
    else:
        pieces = np.unique(pieces, axis=0)
        return pieces


def smallest(x):
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
    b = rotate_piece_cw(a)
    c = rotate_piece_cw(b)
    d = rotate_piece_cw(c)
    e = flip_piece(pieces, flip)
    f = rotate_piece_cw(e)
    g = rotate_piece_cw(f)
    h = rotate_piece_cw(g)
    x = np.stack((a, b, c, d, e, f, g, h), axis=-2)
    
    # Select lexicographically smallest transformation
    y = smallest(x)
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


def grid_to_pieces(horizontal_edges, vertical_edges, opposite):

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


def rotate_grid_cw(horizontal_edges, vertical_edges, opposite):
    # TODO rotate grid CW
    ...


def rotate_grid_ccw(horizontal_edges, vertical_edges, opposite):
    # TODO rotate grid CCW
    ...


def flip_grid(horizontal_edges, vertical_edges, opposite, flip):
    # TODO flip grid
    ...


def canonize_grid(horizontal_edges, vertical_edges, opposite, flip):
    # TODO canonical grid
    ...
