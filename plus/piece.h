
#ifndef JIGSAW_PIECE_H
#define JIGSAW_PIECE_H


#include <algorithm>
#include <cstdint>


/*
    0: flat
    1: male, big, centered
    2: female, big, centered
    3: male, small, centered
    4: female, small, centered
*/


inline constexpr unsigned opposite(unsigned e) {
    switch (e) {
    case 1: return 2;
    case 2: return 1;
    case 3: return 4;
    case 4: return 3;
    }
    return e;
}


inline constexpr unsigned flip(unsigned e) {
    // TODO for asymetric edges, need to be careful!
    return e;
}


struct Piece {
    unsigned edges[4];

    constexpr Piece rotate() const {
        return Piece {edges[1], edges[2], edges[3], edges[0]};
    }

    constexpr Piece flip() const {
        return Piece {::flip(edges[3]), ::flip(edges[2]), ::flip(edges[1]), ::flip(edges[0])};
    }

    constexpr Piece canonical() const {
        Piece ps[8] = {};
        ps[0] = *this;
        ps[1] = ps[0].rotate();
        ps[2] = ps[1].rotate();
        ps[3] = ps[2].rotate();
        ps[4] = flip();
        ps[5] = ps[4].rotate();
        ps[6] = ps[5].rotate();
        ps[7] = ps[6].rotate();
        return *std::min_element(ps, ps + 8);
    }

    constexpr bool operator<(Piece const & other) const {
        for (unsigned i = 0; i < 4; ++i) {
            if (edges[i] < other.edges[i])
                return true;
            if (edges[i] > other.edges[i])
                return false;
        }
        return false;
    }
};


#endif
