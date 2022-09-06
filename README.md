
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


## Getting started

...


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
