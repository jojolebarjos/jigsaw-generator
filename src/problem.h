
#ifndef JIGSAW_PROBLEM_H
#define JIGSAW_PROBLEM_H


#include <cstdint>
#include <map>
#include <vector>

#include "piece.h"


struct Definition {

    unsigned H;
    unsigned W;
    std::map<Piece, unsigned> pieces;
    std::vector<int> horizontal_constraints; // H x (W+1)
    std::vector<int> vertical_constraints; // (H+1) x W

    Definition(unsigned H, unsigned W)
    : H(H)
    , W(W) {

        // Initialize as "no constraint" everywhere
        horizontal_constraints.assign(H * (W + 1), -1);
        vertical_constraints.assign((H + 1) * W, -1);

        // Force perimeter to be flat edges
        for (unsigned y = 0; y < H; ++y) {
            horizontal_constraint(y, 0) = 0;
            horizontal_constraint(y, W) = 0;
        }
        for (unsigned x = 0; x < W; ++x) {
            vertical_constraint(0, x) = 0;
            vertical_constraint(H, x) = 0;
        }
    }

    int & horizontal_constraint(unsigned y, unsigned x) {
        return horizontal_constraints[y * (W + 1) + x];
    }

    int const & horizontal_constraint(unsigned y, unsigned x) const {
        return horizontal_constraints[y * (W + 1) + x];
    }

    int & vertical_constraint(unsigned y, unsigned x) {
        return vertical_constraints[y * W + x];
    }

    int const & vertical_constraint(unsigned y, unsigned x) const {
        return vertical_constraints[y * W + x];
    }
};


struct Problem {

    uint8_t C, N, H, W;

    std::vector<Piece> pieces; // N

    std::vector<uint8_t> count; // C
    std::vector<uint8_t> canonical; // N
    std::vector<uint8_t> accept; // H*W x N
    std::vector<uint8_t> horizontal_match; // N x N
    std::vector<uint8_t> vertical_match; // N x N

    std::vector<uint8_t> stack; // (H+1)*W
    unsigned depth;
    bool has;

    Problem(Definition const & definition) {
        unsigned index;

        // Get canonical set
        // Note: square piece is always registered (used internally)
        std::map<Piece, unsigned> canonical_pieces;
        canonical_pieces[{0, 0, 0, 0}] = 0;
        for (auto const & pair : definition.pieces)
            canonical_pieces[pair.first.canonical()] += pair.second;

        // Generate all transformations
        std::map<Piece, Piece> transformed_pieces;
        for (auto const & pair : canonical_pieces) {
            Piece const & piece = pair.first;
            Piece p = piece;
            transformed_pieces[p] = piece;
            for (unsigned k = 0; k < 3; ++k) {
                p = p.rotate();
                transformed_pieces[p] = piece;
            }
            p = p.flip();
            transformed_pieces[p] = piece;
            for (unsigned k = 0; k < 3; ++k) {
                p = p.rotate();
                transformed_pieces[p] = piece;
            }
        }

        // Get problem size
        C = canonical_pieces.size();
        N = transformed_pieces.size();
        H = definition.H;
        W = definition.W;

        // Map to indices
        std::map<Piece, unsigned> canonical_indices;
        index = 0;
        for (auto const & pair : canonical_pieces)
            canonical_indices[pair.first] = index++;

        // Prepare piece matrix
        std::map<Piece, unsigned> transformed_indices;
        index = 0;
        pieces.resize(N);
        for (auto const & pair : transformed_pieces) {
            transformed_indices[pair.first] = index;
            pieces[index] = pair.first;
            ++index;
        }

        // Prepare canonical matrix
        canonical.resize(N);
        for (Piece const & piece : pieces)
            canonical[transformed_indices[piece]] = canonical_indices[piece.canonical()];

        // Prepare count matrix
        count.resize(C);
        for (auto const & pair : canonical_pieces)
            count[canonical_indices[pair.first]] = pair.second;

        // Prepare matchmaking matrices
        horizontal_match.resize(N * N);
        vertical_match.resize(N * N);
        for (unsigned a = 0; a < N; ++a)
            for (unsigned b = 0; b < N; ++b) {
                horizontal_match[a * N + b] = pieces[a].edges[0] == opposite(pieces[b].edges[2]);
                vertical_match[a * N + b] = pieces[a].edges[3] == opposite(pieces[b].edges[1]);
            }

        // Prepare acceptance matrix
        accept.assign(H * W * N, 1);
        for (unsigned y = 0; y < H; ++y)
            for (unsigned x = 0; x < W; ++x) {
                int constraints[4] = {
                    definition.horizontal_constraint(y, x + 1),
                    (int)opposite((unsigned)definition.vertical_constraint(y, x)),
                    (int)opposite((unsigned)definition.horizontal_constraint(y, x)),
                    definition.vertical_constraint(y + 1, x)
                };
                //printf("(%u, %u) : %d, %d, %d, %d\n", y, x, constraints[0], constraints[1], constraints[2], constraints[3]);
                for (index = 0; index < N; ++index)
                    for (int k = 0; k < 4; ++k)
                        if (constraints[k] >= 0 && constraints[k] != pieces[index].edges[k])
                            accept[(y * W + x) * N + index] = 0;
            }

        // Initialize search
        stack.assign((H + 1) * W, 0);
        depth = W;
        has = false;
    }

    bool next() {

        // Loop until a solution is found
        while (true) {

            // Check whether current piece matches
            if (
                !has &&
                count[canonical[stack[depth]]] &&
                accept[(depth - W) * N + stack[depth]] &&
                horizontal_match[stack[depth - 1] * N + stack[depth]] &&
                vertical_match[stack[depth - W] * N + stack[depth]]
            ) {

                // If board is full, we have a solution
                if (depth == (H + 1) * W - 1) {
                    has = true;
                    return true;
                }
                
                // Otherwise, we need to go deeper
                count[canonical[stack[depth]]] -= 1;
                depth += 1;
                stack[depth] = 0;
                continue;
            }
            has = false;

            // Check whether current depth is exhausted
            while (stack[depth] == N - 1) {

                // Search is complete
                if (depth == W)
                    return false;

                // Backtrack one depth level
                depth -= 1;
                count[canonical[stack[depth]]] += 1;
            }

            // Select next piece
            stack[depth] += 1;
        }
    }
};


#endif
