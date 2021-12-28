# Jigsaw generator

...


## A bit of theory

A piece has 4 edges, each edge having a so-called type.
By design, adjacent pieces must have complementary edge types.
Let us distinguish between two families of edge types:

 1. Symmetric edges, which are their own complement.
    Flat edges, typically used for borders, are the most common example.
    More generally, any shape that is invariant under central [point reflection](https://en.wikipedia.org/wiki/Point_reflection) (i.e. [centrally symmetric](https://en.wikipedia.org/wiki/Centrosymmetry)) are in this family.
 2. Asymmetric edges, which come in pairs.
    Traditionally, most asymmetric edges are designed in a male-female fashion, where one edge has some extrusion.


### Number of unique pieces

Regardless of whether a jigsaw puzzle can be solved, we can estimate how many different pieces can be uniquely created, given the number of edge types.

| q  | p     |
|----|-------|
| 1  | 1     |
| 2  | 6     |
| 3  | 24    |
| 4  | 70    |
| 5  | 165   |
| 6  | 336   |
| 7  | 616   |
| 8  | 1044  |
| 9  | 1665  |
| 10 | 2530  |
| 11 | 3696  |
| 12 | 5226  |
| 13 | 7189  |
| 14 | 9660  |
| 15 | 12720 |
| 16 | 16456 |
| 17 | 20961 |
| 18 | 26334 |
| 19 | 32680 |
| 20 | 40110 |


## References

No easy puzzles: Hardness results for jigsaw puzzles
https://www.sciencedirect.com/science/article/pii/S0304397515001607

Solving Small-piece Jigsaw Puzzles by Growing Consensus
https://openaccess.thecvf.com/content_cvpr_2016/papers/Son_Solving_Small-Piece_Jigsaw_CVPR_2016_paper.pdf



Shotgun Assembly of Labeled Graphs
https://ieeexplore.ieee.org/abstract/document/8119552

Solving jigsaw puzzles by computer
https://link.springer.com/article/10.1007%252FBF02186360

Automatic Reassembly of Three-Dimensional Jigsaw Puzzles
https://www.worldscientific.com/doi/abs/10.1142/S0219467816500091

Automatic Solution of Jigsaw Puzzles
https://link.springer.com/article/10.1007/s10851-013-0454-3

Multiphase Numerical Modeling of Dendritic Solidification for Jigsaw Puzzle Generation
https://n-e-r-v-o-u-s.com/works/puzzles/jigsaw-paper.pdf

Jigsaw puzzle images for steganography
https://www.spiedigitallibrary.org/journals/Optical-Engineering/volume-48/issue-7/077006/Jigsaw-puzzle-images-for-steganography/10.1117/1.3159872.short?SSO=1
