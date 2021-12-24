
#include <algorithm>
#include <cstdio>
#include <set>


struct Piece {
    int edges[4];

    bool is_canonical() const {
        return edges[0] <= edges[1] && edges[0] <= edges[2] && edges[0] <= edges[3];
    }

    void rotate_clockwise() {
        int tmp = edges[0];
        edges[0] = edges[1];
        edges[1] = edges[2];
        edges[2] = edges[3];
        edges[3] = tmp;
    }

    void make_canonical() {
        while (!is_canonical())
            rotate_clockwise();
    }

    bool operator<(Piece const & other) const {
        return std::lexicographical_compare(edges, edges + 4, other.edges, other.edges + 4);
    }
};


int main(int argc, char* argv[]) {

    // In this experiment, we don't associate "male" and "female" jigs
    for (int num_edge_types = 1; num_edge_types <= 20; ++num_edge_types) {

        std::set<Piece> pieces;
        for (int a = 0; a < num_edge_types; ++a)
            for (int b = 0; b < num_edge_types; ++b)
                for (int c = 0; c < num_edge_types; ++c)
                    for (int d = 0; d < num_edge_types; ++d) {
                        Piece p = {a, b, c, d};
                        p.make_canonical();
                        pieces.insert(p);
                    }

        printf("%d: %zu\n", num_edge_types, pieces.size());
    }

    return 0;
}
