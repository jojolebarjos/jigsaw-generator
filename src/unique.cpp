
#include <cstdint>
#include <cstdio>
#include <map>
#include <set>


union Piece {
    uint8_t edges[4];
    uint32_t value;

    Piece clockwise() const {
        return Piece {edges[1], edges[2], edges[3], edges[0]};
    }

    Piece flip() const {
        return Piece {edges[3], edges[2], edges[1], edges[0]};
    }

    Piece canonical() const {
        Piece p = *this;
        Piece minimum = p;
        for (unsigned i = 0; i < 3; ++i) {
            p = p.clockwise();
            if (p < minimum)
                minimum = p;
        }
        return minimum;
    }

    Piece canonical_flipped() const {
        Piece a = flip().canonical();
        Piece b = canonical();
        return std::min(a, b);
        return a < b ? a : b;
    }

    bool has_rotational_symmetry() const {
        return edges[0] == edges[2] && edges[1] == edges[3];
    }

    bool operator<(Piece const & other) const {
        return value < other.value;
    }
};


int main(int argc, char* argv[]) {

    // Print header
    printf("edges,all,canonical,canonical&asymmetric,canonical&asymmetric&flipped");

    // Given the number of distinct edge types
    for (unsigned num_edge_types = 1; num_edge_types <= 20; ++num_edge_types) {

        // Map all possible pieces to their unique equivalent
        // Note: we also consider the corner case where player is allowed to flip the pieces
        std::map<Piece, Piece> canonical;
        std::map<Piece, Piece> canonical_flipped;
        for (unsigned a = 0; a < num_edge_types; ++a)
            for (unsigned b = 0; b < num_edge_types; ++b)
                for (unsigned c = 0; c < num_edge_types; ++c)
                    for (unsigned d = 0; d < num_edge_types; ++d) {
                        Piece p = {(uint8_t)a, (uint8_t)b, (uint8_t)c, (uint8_t)d};
                        canonical[p] = p.canonical();
                        canonical_flipped[p] = p.canonical_flipped();
                    }

        // Get unique pieces
        std::set<Piece> canonical_pieces;
        for (auto & it : canonical)
            canonical_pieces.insert(it.second);

        // Get unique pieces, excluding the ones with rotational symmetry
        uint64_t canonical_asymmetrical_count = 0;
        for (auto & p : canonical_pieces)
            if (!p.has_rotational_symmetry())
                ++canonical_asymmetrical_count;

        // Get unique pieces, excluding flipped ones
        std::set<Piece> canonical_flipped_pieces;
        for (auto & it : canonical_flipped)
            canonical_flipped_pieces.insert(it.second);

        // Get unique pieces, excluding both symmetries and flips
        uint64_t canonical_flipped_asymmetrical_count = 0;
        for (auto & p : canonical_flipped_pieces)
            if (!p.has_rotational_symmetry())
                ++canonical_flipped_asymmetrical_count;

        // Print
        printf("%u,%zu,%zu,%llu,%llu\n",
            num_edge_types,
            canonical.size(),
            canonical_pieces.size(),
            canonical_asymmetrical_count,
            canonical_flipped_asymmetrical_count
        );
    }

    return 0;
}
