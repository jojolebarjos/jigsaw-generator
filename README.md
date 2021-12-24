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
| 2  | 9     |
| 3  | 36    |
| 4  | 100   |
| 5  | 225   |
| 6  | 441   |
| 7  | 784   |
| 8  | 1296  |
| 9  | 2025  |
| 10 | 3025  |
| 11 | 4356  |
| 12 | 6084  |
| 13 | 8281  |
| 14 | 11025 |
| 15 | 14400 |
| 16 | 18496 |
| 17 | 23409 |
| 18 | 29241 |
| 19 | 36100 |
| 20 | 44100 |


## References

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
