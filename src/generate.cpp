
#include <algorithm>
#include <cstdio>
#include <map>
#include <random>
#include <vector>


struct Piece {

    // 0 = a = East
    // 1 = b = North
    // 2 = c = West
    // 3 = d = South

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

    inline int & operator[](int i) {
        return edges[i];
    }

    inline int const & operator[](int i) const {
        return edges[i];
    }
};


struct PiecePool {

    int num_edge_types;
    size_t num_pieces;
    Piece * pieces;
    bool * ownership;
    bool ** mapping;

    inline bool*& get_mapping(int a, int b, int c, int d) {
        int index = (((size_t)a * num_edge_types + b) * num_edge_types + c) * num_edge_types + d;
        return mapping[index];
    }

    PiecePool(int num_edge_types) : num_edge_types(num_edge_types) {

        // Allocate full list of pieces, including rotation
        // Note: ownership is obviously too big, but we don't bother...
        size_t count = (size_t)num_edge_types * num_edge_types * num_edge_types * num_edge_types;
        ownership = new bool[count];
        mapping = new bool*[count];

        // Get unique set of pieces
        std::map<Piece, size_t> map;
        for (int a = 0; a < num_edge_types; ++a)
            for (int b = 0; b < num_edge_types; ++b)
                for (int c = 0; c < num_edge_types; ++c)
                    for (int d = 0; d < num_edge_types; ++d) {
                        Piece p = {a, b, c, d};
                        Piece q = p;
                        q.make_canonical();
                        auto it = map.find(q);
                        size_t index;
                        if (it == map.end()) {
                            index = map.size();
                            map[p] = index;
                        } else {
                            index = it->second;
                        }
                        get_mapping(a, b, c, d) = ownership + index;
                    }
        num_pieces = map.size();

        // Starts empty
        fill(false);
    }

    ~PiecePool() {
        delete ownership;
        delete mapping;
    }

    void fill(bool value) {
        for (size_t i = 0; i < num_pieces; ++i)
            ownership[i] = value;
    }

    inline bool & operator()(int a, int b, int c, int d) {
        return *get_mapping(a, b, c, d);
    }
    
    inline bool & operator()(Piece const & piece) {
        return (*this)(piece[0], piece[1], piece[2], piece[3]);
    }
};


struct ConstraintIterator {
    int num_edge_types;
    Piece constraint;
    Piece piece;
    bool has;

    ConstraintIterator(int num_edge_types, Piece constraint) : num_edge_types(num_edge_types), constraint(constraint), piece(constraint), has(true) {
        for (int i = 0; i < 4; ++i)
            if (constraint[i] < 0)
                piece[i] = 0;
    }

    inline Piece & operator*() {
        return piece;
    }

    inline Piece * operator->() {
        return &piece;
    }

    inline operator bool() const {
        return has;
    }

    void next() {
        for (int i = 0; i < 4; ++i) {
            if (constraint[i] < 0) {
                ++piece[i];
                if (piece[i] < num_edge_types)
                    return;
                piece[i] = 0;
            }
        }
        has = false;
    }
};


struct PiecePoolIterator {
    PiecePool & pool;
    ConstraintIterator it;

    PiecePoolIterator(PiecePool & pool, Piece constraint)
    : pool(pool)
    , it(pool.num_edge_types, constraint)
    {
        while (it && !pool(*it))
            it.next();
    }

    inline Piece & operator*() {
        return it.piece;
    }

    inline Piece * operator->() {
        return &it.piece;
    }

    inline operator bool() const {
        return it;
    }

    void next() {
        it.next();
        while (it && !pool(*it))
            it.next();
    }
};


//std::default_random_engine generator;
//std::uniform_int_distribution<int> distribution(1,6);


void generate(int num_edge_types, int height, int width) {

    // Allow all combinations
    PiecePool pool(num_edge_types);
    pool.fill(true);

    // There is no constraint on the first piece
    std::vector<PiecePoolIterator> stack;
    stack.emplace_back(pool, Piece{-1, -1, -1, -1});

    // Loop until all combinations have been tested
    while (true) {
        PiecePoolIterator & iterator = stack.back();

        // If there is no more candidate at this location, roll back one step
        if (!iterator) {
            stack.pop_back();
            //printf("POP\n");

            // Is search complete?
            if (stack.empty())
                break;

            // Otherwise, advance iterator
            PiecePoolIterator & parent_iterator = stack.back();
            pool(*parent_iterator) = true;
            parent_iterator.next();
            continue;
        }

        // Select iterator piece
        int index = stack.size();
        int x = index % width;
        int y = index / width;
        pool(*iterator) = false;
        //printf("PUSH (%d, %d, %d, %d) at (%d, %d)\n", (*iterator)[0], (*iterator)[1], (*iterator)[2], (*iterator)[3], x, y);

        // Is jigsaw complete?
        if (index == width * height) {

            // Print as JSON line
            printf("[");
            for (int i = 0; i < height; ++i) {
                if (i > 0)
                    printf(",");
                printf("[");
                for (int j = 0; j < width; ++j) {
                    if (j > 0)
                        printf(",");
                    Piece & p = *stack[i * width + j];
                    printf("[%d,%d,%d,%d]", p[0], p[1], p[2], p[3]);
                }
                printf("]");
            }
            printf("]\n");

            // Continue search
            pool(*iterator) = true;
            iterator.next();
            continue;
        }

        // Create iterator for the next location
        Piece constraint = {-1, -1, -1, -1};
        if (y > 0)
            constraint[1] = (*stack[index - width])[3];
        if (x > 0)
            constraint[2] = (*stack[index - 1])[0];
        stack.emplace_back(pool, constraint);
    }
    
    //printf("End of search.\n");
}


int main(int argc, char* argv[]) {
    /*
    PiecePool pool(8);
    printf("%zu\n", pool.num_pieces);
    
    pool(0, 1, 2, 3) = true;
    printf("%d\n", pool(0, 1, 2, 3));
    printf("%d\n", pool(1, 2, 3, 0));
    printf("%d\n", pool(3, 2, 1, 0));
    */
    /*
    PiecePool pool(3);
    pool(0, 0, 0, 0) = true;
    pool(0, 1, 0, 0) = true;
    pool(0, 2, 0, 0) = true;
    pool(0, 1, 1, 0) = true;
    PiecePoolIterator it(pool, {-1, 1, -1, -1});

    while (it) {
        printf("%d, %d, %d, %d\n", (*it)[0], (*it)[1], (*it)[2], (*it)[3]);
        it.next();
    }
    */
    
    generate(2, 3, 3);
    
    return 0;
}
