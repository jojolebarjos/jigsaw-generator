
#include <cstdint>
#include <cstdio>
#include <map>
#include <set>


union Piece {
    uint8_t edges[4];
    uint32_t value;

    Piece clockwise() const {
        Piece out = {edges[1], edges[2], edges[3], edges[0]};
        return out;
    }

    bool operator<(Piece const & other) const {
        return value < other.value;
    }
};


int main(int argc, char* argv[]) {

    // In this experiment, we don't associate "male" and "female" jigs
    for (int num_edge_types = 1; num_edge_types <= 20; ++num_edge_types) {

        // Map all possible pieces to their unique equivalent
        std::map<Piece, Piece> canonical;
        for (unsigned a = 0; a < num_edge_types; ++a)
            for (unsigned b = 0; b < num_edge_types; ++b)
                for (unsigned c = 0; c < num_edge_types; ++c)
                    for (unsigned d = 0; d < num_edge_types; ++d) {
                        Piece p = {a, b, c, d};
                        canonical[p] = p;
                        Piece q = p;
                        for (int i = 0; i < 4; ++i) {
                            q = q.clockwise();
                            if (q < p)
                                break;
                        }
                        canonical[p] = canonical[q];
                    }

        // Get unique pieces
        std::set<Piece> pieces;
        for (auto & it : canonical)
            pieces.insert(it.second);

        printf("%d: %zu\n", num_edge_types, pieces.size());
    }

    return 0;
}
