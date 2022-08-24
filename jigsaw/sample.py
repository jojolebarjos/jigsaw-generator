
import numpy as np


def sample_random_grid(H, W, K, *, generator=None, border=0):

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


def sample_unique_grid(sampler, opposite, flip, *, max_iterations=None, constraints=None, on_sample=None):
    if on_sample == "tqdm":
        from tqdm import tqdm
    while True:
        horizontal_edges, vertical_edges = sample()
        pieces = grid_to_pieces(horizontal_edges, vertical_edges, opposite)
        try:
            return get_unique_solution(H, W, pieces, opposite, flip, constraints="border")
        except:
            continue
        finally:
            on_sample()
