
# Jigsaw generator

This is a small experiment where jigsaw puzzles are randomly generated.

We focus on regular grids, where all pieces are squares of the same size.
A piece has 4 edges, selected from a small set of shapes, namely edge types.
By design, adjacent pieces must have complementary edge types.
Having only a few edge types increases the combinations, as more pieces can connect together.

Moreover, the goal is to make apictoral jigsaw puzzles out of transparent acrylic.
This means that front and back faces cannot be distinguished by looking at the piece.
In practice, this not only increases the number of combinations, it also requires a different intuition from usual puzzles.

The main design constraint is to have exactly one solution, if we ignore the rotation and flip of the whole board.
This property opens various strategies, as well as making the problem well defined (i.e. you search THE solution).
However, proving that there is a single solution is likely to be NP-hard.
Hence, the approach here is to brute force all combinations, effectively counting them.

<img src="image/attempt_01_12_001.jpg" width="800" height="600" alt="Transparent acrylic jigsaw puzzle">


## Getting started

First of all, this project is meant as an experiment, and is therefore not expected to be flexible, nor robust.
Still, feel free to play with the provided helpers!

In this project, jigsaw puzzles are 2D grids of square pieces, where edges belong to a finite (and typically small) set.
While the helpers are typically agnostic about the actual shape of the edge types, this discussion will assume that the definitions from `jigsaw.default` are used.
This provides a definition for `opposite` and `flip`, the two operations required to infer how edges can connect.

Currently, low-level functions are provided, in order to manipulate 2D grids.
There are two main representations, depending on the context:

 * __Edge-wise__, referred as _grid_, which is a pair of 2D arrays, representing horizontal and vertical edges, of shape H x (W + 1) and (H + 1) x W, respectively.
 * __Piece-wise__, referred as _pieces_, which is a single 3D array, representing a 2D array of pieces, of shape H x W x 4.

While the former has the advantage of not duplicating any information (and therefore should be usually preferred), the latter is used during the search, as pieces are manipulated explicitly.
A basic usage is showcased in [`getting_started.ipynb`](getting_started.ipynb).

A more scalable approach is provided in [`attempt_01.ipynb`](attempt_01.ipynb); as the name implies, this is not well-tested, but it has proved sufficient until now.
The overall flow is as follow:

 1. Define a sampler, which is a parameter-less functor that will return a new random grid.
 2. Optionally define custom validators; usually if this is easier to implement a rule as a test instead of a sampling strategy.
 3. Search for a sample that is successfully validated.
    The uniqueness of the solution is usually the last validator to be applied.

The number of solution roughly depends on the ratio of the number of edge types and the number of pieces.
On the one hand, if there are too many edge types, then there is almost surely a single (and often easy) solution.
On the other hand, if there are too few edge types, then there are almost surely many solutions; if uniqueness is requested, this will not converge.

I believe there is no bulletproof strategy here, finding the right balance is trial-and-error.
The key challenge is to make a puzzle that is enjoyable, and not just pure brute-force.
Remembering which combinations of pieces have been tested may very well be a nightmare, so be sure to add a "strategy" in your design.


## Attempts

Here are the provided design attempts.

<details>
  <summary>01.01 &mdash; 3x3 basic</summary>

  <img src="image/01_01_3x3_basic.svg" width="300" height="300" alt="01.01 - 3x3 basic">
</details>

<details>
  <summary>01.02 &mdash; 3x3</summary>

  <img src="image/01_02_3x3.svg" width="300" height="300" alt="01.02 - 3x3">
</details>

<details>
  <summary>01.03 &mdash; 3x3 skewed</summary>

  <img src="image/01_03_3x3_skewed.svg" width="300" height="300" alt="01.03 - 3x3 skewed">
</details>

<details>
  <summary>01.04 &mdash; 3x3 skewed</summary>

  <img src="image/01_04_3x3_twisted.svg" width="300" height="300" alt="01.04 - 3x3 twisted">
</details>

<details>
  <summary>01.05 &mdash; 4x4</summary>

  <img src="image/01_05_4x4.svg" width="400" height="400" alt="01.05 - 4x4">
</details>

<details>
  <summary>01.06 &mdash; 4x4 twisted</summary>

  <img src="image/01_06_4x4_twisted.svg" width="400" height="400" alt="01.06 - 4x4 twisted">
</details>

<details>
  <summary>01.07 &mdash; 4x4 with 8 corners</summary>

  <img src="image/01_07_4x4_cross_constrained.svg" width="400" height="400" alt="01.07 - 4x4 with 8 corners">
</details>

<details>
  <summary>01.08 &mdash; 4x4 with 5th corner</summary>

  <img src="image/01_08_4x4_5th.svg" width="400" height="400" alt="01.08 - 4x4 with 5th corner">
</details>

<details>
  <summary>01.09 &mdash; 4x4 split</summary>

  <img src="image/01_09_4x4_split.svg" width="400" height="400" alt="01.09 - 4x4 split">
</details>

<details>
  <summary>01.10 &mdash; 4x4 snake</summary>

  <img src="image/01_10_4x4_snake.svg" width="400" height="400" alt="01.10 - 4x4 snake">
</details>

<details>
  <summary>01.11 &mdash; 5x5 with square center</summary>

  <img src="image/01_11_5x5_center.svg" width="500" height="500" alt="01.11 - 5x5 with square center">
</details>

<details>
  <summary>01.12 &mdash; 5x5 with 5th corner</summary>

  <img src="image/01_12_5x5_5th.svg" width="500" height="500" alt="01.12 - 5x5 with 5th corner">
</details>

<details>
  <summary>02.01 &mdash; 4x4 with 8 corners</summary>

  <img src="image/02_01_4x4_cross.svg" width="400" height="400" alt="02.01 - 4x4 with 8 corners">
</details>

<details>
  <summary>02.02 &mdash; 5x5 with nested corners</summary>

  <img src="image/02_02_5x5_nested.svg" width="500" height="500" alt="02.02 - 5x5 with nested corners">
</details>

<details>
  <summary>02.03 &mdash; 4x4 with pure square</summary>

  <img src="image/02_03_4x4_pure.svg" width="400" height="400" alt="02.03 - 4x4 with pure square">
</details>

<details>
  <summary>02.04 &mdash; 4x4 with more edges</summary>

  <img src="image/02_04_4x4_edge.svg" width="400" height="400" alt="02.04 - 4x4 with more edges">
</details>

<details>
  <summary>02.05 &mdash; 4x4 with 6 corners</summary>

  <img src="image/02_05_4x4_6_corners.svg" width="400" height="400" alt="02.05 - 4x4 with 6 corners">
</details>

<details>
  <summary>02.06 &mdash; 4x4 corners-only</summary>

  <img src="image/02_06_4x4_only_corners.svg" width="400" height="400" alt="02.06 - 4x4 corners-only">
</details>

<details>
  <summary>02.07 &mdash; 4x4 cycle</summary>

  <img src="image/02_07_4x4_cycle.svg" width="400" height="400" alt="02.07 - 4x4 cycle">
</details>


## About laser cutters

This whole project came to be since I got the opportunity to use some laser cutter, a Trotec Speedy 300.
I am by no mean an expert, but cutting acrylic is somewhat straightforward; you just need to find a balance between speed and power, and vendors usually provide hints about typical values.

However, it seems that there are two main categories of acrylic: extruded (XT) and cast (GS).
While the former is cheaper, the latter yields better-looking results.

A few references:

 * [Trotec: Speedy 300 Operating Manual](https://www.troteclaser.com/static/pdf/speedy-300/operating-manual-Speedy-300-8066-en-us.pdf)
 * [Trotec: Comparison of cast and extruded acrylic](https://www.trotec-materials.com/material-usage-hints/extruded-vs-cast-acrylic)
 * [3mm cast acrylic glass used](https://www.amazon.de/-/en/gp/product/B08B8TLHPP/)

<img src="image/attempt_01_cut_001.jpg" width="400" height="225" alt="Trotec Speedy 300 cutting the first attempt">


## References

Even though these were not used in the final code, this may be of interest:

 * _Unique reconstruction threshold for random jigsaw puzzles_, Nenadov et al., 2016 ([link](https://arxiv.org/abs/1605.03043))
 * _Even 1 × n Edge-Matching and Jigsaw Puzzles are Really Hard_, Bosboom et al., 2016 ([link](https://arxiv.org/abs/1701.00146))
 * _No easy puzzles: Hardness results for jigsaw puzzles_, Brand, 2015 ([link](https://www.sciencedirect.com/science/article/pii/S0304397515001607))
 * _Solving Small-piece Jigsaw Puzzles by Growing Consensus_, Son et al., 2016 ([pdf](https://openaccess.thecvf.com/content_cvpr_2016/papers/Son_Solving_Small-Piece_Jigsaw_CVPR_2016_paper.pdf))
 * _Shotgun Assembly of Labeled Graphs_, Mossel and Ross, 2017 ([link](https://ieeexplore.ieee.org/abstract/document/8119552))
 * _Solving jigsaw puzzles by computer_, Wolfson et al., 1988 ([link](https://link.springer.com/article/10.1007%252FBF02186360))
 * _Automatic Reassembly of Three-Dimensional Jigsaw Puzzles_, Grim et al., 2016 ([link](https://www.worldscientific.com/doi/abs/10.1142/S0219467816500091))
 * _Automatic Solution of Jigsaw Puzzles_, Hoff and Olver, 2014 ([link](https://link.springer.com/article/10.1007/s10851-013-0454-3))
