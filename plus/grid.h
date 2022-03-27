
#ifndef JIGSAW_GRID_H
#define JIGSAW_GRID_H


#include <cstdint>
#include <cstdio>
#include <map>
#include <random>
#include <vector>

#include "piece.h"


struct Grid {
    unsigned H, W;
    std::vector<unsigned> horizontal; // H x (W + 1)
    std::vector<unsigned> vertical; // (H + 1) x W

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
            vertical[y * W + x],
            horizontal[y * (W + 1) + x],
            vertical[(y + 1) * W + x]
        };
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
};


#endif
