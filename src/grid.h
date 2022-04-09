
#ifndef JIGSAW_GRID_H
#define JIGSAW_GRID_H


#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <map>
#include <random>
#include <vector>

#include "piece.h"
#include "problem.h"


struct Grid {
    unsigned H, W;
    std::vector<unsigned> horizontal; // H x (W + 1)
    std::vector<unsigned> vertical; // (H + 1) x W
    // Note: edge is "seen" from left/top

    Grid() : H(0), W(0) {}

    Grid(unsigned H, unsigned W)
    : H(H)
    , W(W)
    {
        horizontal.assign(H * (W + 1), 0);
        vertical.assign((H + 1) * W, 0);
    }

    template <typename Sampler>
    void randomize(Sampler & sampler) {
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 1; x < W; ++x)
                horizontal[y * (W + 1) + x] = sampler();
        for (unsigned y = 1; y < H; ++y)
            for (unsigned x = 0; x < W; ++x)
                vertical[y * W + x] = sampler();
    }

    Piece at(unsigned y, unsigned x) const {
        return {
            horizontal[y * (W + 1) + x + 1],
            opposite(vertical[y * W + x]),
            opposite(horizontal[y * (W + 1) + x]),
            vertical[(y + 1) * W + x]
        };
    }

    std::vector<Piece> pieces() const {
        std::vector<Piece> result(H * W);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x)
                result[y * W + x] = at(y, x);
        return result;
    }

    bool operator<(Grid const & other) const {
        std::vector<Piece> left = pieces();
        std::vector<Piece> right = other.pieces();
        return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
    }

    Grid rotate() const {
        Grid result(W, H);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x <= W; ++x)
                result.vertical[x * H + H - 1 - y] = horizontal[y * (W + 1) + x];
        for (unsigned y = 0; y <= H; ++y)
            for (unsigned x = 0; x < W; ++x)
                result.horizontal[x * (H + 1) + H - y] = opposite(vertical[y * W + x]);
        return result;
    }

    Grid flip() const {
        Grid result(W, H);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x <= W; ++x)
                result.vertical[x * H + y] = horizontal[y * (W + 1) + x];
        for (unsigned y = 0; y <= H; ++y)
            for (unsigned x = 0; x < W; ++x)
                result.horizontal[x * (H + 1) + y] = vertical[y * W + x];
        return result;
    }

    Grid canonical() const {
        Grid gs[8];
        gs[0] = *this;
        gs[1] = gs[0].rotate();
        gs[2] = gs[1].rotate();
        gs[3] = gs[2].rotate();
        gs[4] = flip();
        gs[5] = gs[4].rotate();
        gs[6] = gs[5].rotate();
        gs[7] = gs[6].rotate();
        return *std::min_element(gs, gs + 8);
    }

    void print() const {
        printf("[");
        for (unsigned y = 0; y < H; ++y) {
            if (y > 0)
                printf(", ");
            printf("[");
            for (unsigned x = 0; x < W; ++x) {
                if (x > 0)
                    printf(", ");
                Piece p = at(y, x);
                printf("[%u, %u, %u, %u]", p.edges[0], p.edges[1], p.edges[2], p.edges[3]);
            }
            printf("]");
        }
        printf("]\n");
    }

    void print_canonical() const {
        std::map<Piece, unsigned> count;
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x)
                count[at(y, x).canonical()] += 1;
        for (auto const & pair : count)
            printf("[%u, %u, %u, %u]: %u\n", pair.first.edges[0], pair.first.edges[1], pair.first.edges[2], pair.first.edges[3], pair.second);
    }

    bool has_duplicate() const {
        std::map<Piece, unsigned> count;
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x) {
                Piece p = at(y, x).canonical();
                if (++count[p] > 1)
                    return true;
            }
        return false;
    }

    Definition to_definition() const {
        Definition definition(H, W);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x)
                definition.pieces[at(y, x)] += 1;
        return definition;
    }

    static Grid from_problem(Problem const & problem) {
        unsigned H = problem.H;
        unsigned W = problem.W;
        Grid grid(H, W);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x) {
                unsigned depth = (y + 1) * W + x;
                Piece piece = problem.pieces[problem.stack[depth]];
                if (x == 0)
                    grid.horizontal[y * (W + 1) + x + 1] = opposite(piece.edges[2]);
                grid.horizontal[y * (W + 1) + x + 1] = piece.edges[0];
                if (y == 0)
                    grid.vertical[y * W + x] = opposite(piece.edges[1]);
                grid.vertical[(y + 1) * W + x] = piece.edges[3];
            }
        return grid;
    }
};


#endif
