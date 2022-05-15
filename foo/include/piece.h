
#ifndef JIGSAW_PIECE_H
#define JIGSAW_PIECE_H


#include <algorithm>
#include <cstdint>


enum Type : unsigned {
    TYPE_FLAT = 0,
    TYPE_CENTERED_BIG_MALE = 1,
    TYPE_CENTERED_BIG_FEMALE = 2,
    TYPE_CENTERED_MEDIUM_MALE = 3,
    TYPE_CENTERED_MEDIUM_FEMALE = 4,
    TYPE_CENTERED_SMALL_MALE = 5,
    TYPE_CENTERED_SMALL_FEMALE = 6,
    TYPE_RIGHT_BIG_MALE = 7,
    TYPE_RIGHT_BIG_FEMALE = 8,
    TYPE_RIGHT_MEDIUM_MALE = 9,
    TYPE_RIGHT_MEDIUM_FEMALE = 10,
    TYPE_RIGHT_SMALL_MALE = 11,
    TYPE_RIGHT_SMALL_FEMALE = 12,
    TYPE_LEFT_BIG_MALE = 13,
    TYPE_LEFT_BIG_FEMALE = 14,
    TYPE_LEFT_MEDIUM_MALE = 15,
    TYPE_LEFT_MEDIUM_FEMALE = 16,
    TYPE_LEFT_SMALL_MALE = 17,
    TYPE_LEFT_SMALL_FEMALE = 18,
    TYPE_DOUBLE_BIG_MALE = 19,
    TYPE_DOUBLE_BIG_FEMALE = 20,
    TYPE_DOUBLE_MEDIUM_MALE = 21,
    TYPE_DOUBLE_MEDIUM_FEMALE = 22,
    TYPE_DOUBLE_SMALL_MALE = 23,
    TYPE_DOUBLE_SMALL_FEMALE = 24,
    TYPE_TWISTED_BIG_MALE = 25,
    TYPE_TWISTED_BIG_FEMALE = 26,
    TYPE_TWISTED_MEDIUM_MALE = 27,
    TYPE_TWISTED_MEDIUM_FEMALE = 28,
    TYPE_TWISTED_SMALL_MALE = 29,
    TYPE_TWISTED_SMALL_FEMALE = 30,
};


inline constexpr unsigned opposite(unsigned e) {
    switch (e) {

    // Centered
    case TYPE_CENTERED_BIG_MALE: return TYPE_CENTERED_BIG_FEMALE;
    case TYPE_CENTERED_BIG_FEMALE: return TYPE_CENTERED_BIG_MALE;
    case TYPE_CENTERED_MEDIUM_MALE: return TYPE_CENTERED_MEDIUM_FEMALE;
    case TYPE_CENTERED_MEDIUM_FEMALE: return TYPE_CENTERED_MEDIUM_MALE;
    case TYPE_CENTERED_SMALL_MALE: return TYPE_CENTERED_SMALL_FEMALE;
    case TYPE_CENTERED_SMALL_FEMALE: return TYPE_CENTERED_SMALL_MALE;

    // Right-aligned
    case TYPE_RIGHT_BIG_MALE: return TYPE_LEFT_BIG_FEMALE;
    case TYPE_RIGHT_BIG_FEMALE: return TYPE_LEFT_BIG_MALE;
    case TYPE_RIGHT_MEDIUM_MALE: return TYPE_LEFT_MEDIUM_FEMALE;
    case TYPE_RIGHT_MEDIUM_FEMALE: return TYPE_LEFT_MEDIUM_MALE;
    case TYPE_RIGHT_SMALL_MALE: return TYPE_LEFT_SMALL_FEMALE;
    case TYPE_RIGHT_SMALL_FEMALE: return TYPE_LEFT_SMALL_MALE;

    // Left-aligned
    case TYPE_LEFT_BIG_MALE: return TYPE_RIGHT_BIG_FEMALE;
    case TYPE_LEFT_BIG_FEMALE: return TYPE_RIGHT_BIG_MALE;
    case TYPE_LEFT_MEDIUM_MALE: return TYPE_RIGHT_MEDIUM_FEMALE;
    case TYPE_LEFT_MEDIUM_FEMALE: return TYPE_RIGHT_MEDIUM_MALE;
    case TYPE_LEFT_SMALL_MALE: return TYPE_RIGHT_SMALL_FEMALE;
    case TYPE_LEFT_SMALL_FEMALE: return TYPE_RIGHT_SMALL_MALE;

    // Doubled (symmetric)
    case TYPE_DOUBLE_BIG_MALE: return TYPE_DOUBLE_BIG_FEMALE;
    case TYPE_DOUBLE_BIG_FEMALE: return TYPE_DOUBLE_BIG_MALE;
    case TYPE_DOUBLE_MEDIUM_MALE: return TYPE_DOUBLE_MEDIUM_FEMALE;
    case TYPE_DOUBLE_MEDIUM_FEMALE: return TYPE_DOUBLE_MEDIUM_MALE;
    case TYPE_DOUBLE_SMALL_MALE: return TYPE_DOUBLE_SMALL_FEMALE;
    case TYPE_DOUBLE_SMALL_FEMALE: return TYPE_DOUBLE_SMALL_MALE;

    // Twisted (i.e. doubled, but with gender-swap)
    case TYPE_TWISTED_BIG_MALE: return TYPE_TWISTED_BIG_MALE;
    case TYPE_TWISTED_BIG_FEMALE: return TYPE_TWISTED_BIG_FEMALE;
    case TYPE_TWISTED_MEDIUM_MALE: return TYPE_TWISTED_MEDIUM_MALE;
    case TYPE_TWISTED_MEDIUM_FEMALE: return TYPE_TWISTED_MEDIUM_FEMALE;
    case TYPE_TWISTED_SMALL_MALE: return TYPE_TWISTED_SMALL_MALE;
    case TYPE_TWISTED_SMALL_FEMALE: return TYPE_TWISTED_SMALL_FEMALE;

    }
    return e;
}


inline constexpr unsigned flip(unsigned e) {
    switch (e) {

    // Right-aligned
    case TYPE_RIGHT_BIG_MALE: return TYPE_LEFT_BIG_MALE;
    case TYPE_RIGHT_BIG_FEMALE: return TYPE_LEFT_BIG_FEMALE;
    case TYPE_RIGHT_MEDIUM_MALE: return TYPE_LEFT_MEDIUM_MALE;
    case TYPE_RIGHT_MEDIUM_FEMALE: return TYPE_LEFT_MEDIUM_FEMALE;
    case TYPE_RIGHT_SMALL_MALE: return TYPE_LEFT_SMALL_MALE;
    case TYPE_RIGHT_SMALL_FEMALE: return TYPE_LEFT_SMALL_FEMALE;

    // Left-aligned
    case TYPE_LEFT_BIG_MALE: return TYPE_RIGHT_BIG_MALE;
    case TYPE_LEFT_BIG_FEMALE: return TYPE_RIGHT_BIG_FEMALE;
    case TYPE_LEFT_MEDIUM_MALE: return TYPE_RIGHT_MEDIUM_MALE;
    case TYPE_LEFT_MEDIUM_FEMALE: return TYPE_RIGHT_MEDIUM_FEMALE;
    case TYPE_LEFT_SMALL_MALE: return TYPE_RIGHT_SMALL_MALE;
    case TYPE_LEFT_SMALL_FEMALE: return TYPE_RIGHT_SMALL_FEMALE;

    // Twisted (i.e. doubled, but with gender-swap)
    case TYPE_TWISTED_BIG_MALE: return TYPE_TWISTED_BIG_FEMALE;
    case TYPE_TWISTED_BIG_FEMALE: return TYPE_TWISTED_BIG_MALE;
    case TYPE_TWISTED_MEDIUM_MALE: return TYPE_TWISTED_MEDIUM_FEMALE;
    case TYPE_TWISTED_MEDIUM_FEMALE: return TYPE_TWISTED_MEDIUM_MALE;
    case TYPE_TWISTED_SMALL_MALE: return TYPE_TWISTED_SMALL_FEMALE;
    case TYPE_TWISTED_SMALL_FEMALE: return TYPE_TWISTED_SMALL_MALE;
    }
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
