
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <map>


// Size of the grid
// Note: exhaustive search has exponential complexity!
constexpr unsigned WIDTH = 4;
constexpr unsigned HEIGHT = 4;

// How many edge types
// Note: in this version, type 0 is symmetric, remaining types comes in pair (i.e. male-female);
//       however, you can edit how adjacency constraints are defined, later in the code
constexpr unsigned TYPES = 5;

// How many bits are needed to store these types
constexpr unsigned BITS = 3;
constexpr unsigned MASK = (1 << BITS) - 1;
constexpr unsigned NUM = (1 << BITS) * (1 << BITS) * (1 << BITS) * (1 << BITS);
typedef uint16_t Piece;


// Some tables are pre-computed and used globally
Piece canonical[NUM];
bool right[NUM][NUM];
bool below[NUM][NUM];


// Solution container, used to group solution by their piece set
struct Solution {

    // The actual solution
    Piece stack[WIDTH * HEIGHT];

    // Associated canonical set of pieces
    Piece id[WIDTH * HEIGHT];

    Solution(Piece s[]) {
        for (unsigned i = 0; i < WIDTH * HEIGHT; ++i) {
            stack[i] = s[i];
            id[i] = canonical[s[i]];
        }
        std::sort(id, id + WIDTH * HEIGHT);
    }

    bool operator<(Solution const & other) const {
        return std::lexicographical_compare(id, id + WIDTH * HEIGHT, other.id, other.id + WIDTH * HEIGHT);
    }
};


// Exhaustive search of the given set of pieces
// Note: pieces are placed one-by-one, starting from upper-left corner and growing row-by-row
template <typename Callback>
void search(bool has[], Callback & callback) {

    // Recurse, using stack for efficiency
    Piece stack[WIDTH * HEIGHT];
    unsigned index = 0;
    stack[0] = 0;

    // Check if we have this piece
check:
    if (has[canonical[stack[index]]]) {

        // Check that this piece is allowed there
        Piece piece = stack[index];
        bool fit = true;
        unsigned x = index % WIDTH;
        unsigned y = index / WIDTH;

        // An edge is flat if and only if it is a border
        if (!(
            (x == WIDTH  - 1) == ((piece >> (0 * BITS) & MASK) == 0) &&
            (y ==          0) == ((piece >> (1 * BITS) & MASK) == 0) &&
            (x ==          0) == ((piece >> (2 * BITS) & MASK) == 0) &&
            (y == HEIGHT - 1) == ((piece >> (3 * BITS) & MASK) == 0)
        ))
            fit = false;

        // Edge must match piece on the left side
        if (x > 0 && !right[stack[index - 1]][piece])
            fit = false;

        // Edge must match piece above
        if (y > 0 && !below[stack[index - WIDTH]][piece])
            fit = false;

        // We can place it
        if (fit) {

            // Is jigsaw complete?
            if (index == WIDTH * HEIGHT - 1) {
                if (!callback(stack))
                    return;
                goto inc;
            }

            // Otherwise, push piece on stack
            has[canonical[stack[index]]] = false;
            ++index;
            stack[index] = 0;
            goto check;
        }
    }

    // Check whether this depth is exhausted
inc:
    if (stack[index] == NUM - 1) {

        // That was the highest level, search is therefore complete
        if (index == 0)
            return;

        // Pop from stack
        --index;
        has[canonical[stack[index]]] = true;
        goto inc;
    }

    // Print a dot each time the highest level of the loop is incremented, so that we see how much time we need
    if (index == 0)
        putc('.', stderr);

    // Increment piece
    ++stack[index];
    goto check;
}


bool print(Piece const stack[]) {
    putchar('[');
    for (unsigned i = 0; i < WIDTH * HEIGHT; ++i) {
        if (i > 0)
            putchar(',');
        Piece p = stack[i];
        printf("%u,%u,%u,%u", p & MASK, p >> BITS & MASK, p >> (2 * BITS) & MASK, p >> (3 * BITS) & MASK);
    }
    printf("]\n");
    return true;
}


// Collected data
uint64_t total = 0;
std::map<Solution, uint64_t> counter;

bool collect(Piece stack[]) {
    Solution solution(stack);
    ++total;
    ++counter[solution];
    /*
    if (total % 100000 == 0 || true) {
        fprintf(stderr, "%llu (%zu)\n", total, counter.size());
        for (unsigned i = 0; i < WIDTH; ++i)
            fprintf(stderr, "  %u", solution.stack[i]);
        fprintf(stderr, "\n");
    }*/
    return true;
}


int main(int argc, char* argv[]) {

    //
    // PRECOMPUTED TABLES
    //

    // Generate mapping to unique pieces
    for (unsigned p = 0; p < NUM; ++p) {
        canonical[p] = p;
        Piece q = p;
        for (int i = 0; i < 4; ++i) {
            q = q >> BITS | (q & MASK) << (3 * BITS);
            if (q < p)
                break;
        }
        canonical[p] = canonical[q];
    }

    // Whether two pieces can be together
    for (unsigned a = 0; a < NUM; ++a)
        for (unsigned b = 0; b < NUM; ++b) {
            unsigned a_right = a & MASK;
            unsigned b_left = b >> (2 * BITS) & MASK;
            unsigned a_below = a >> (3 * BITS) & MASK;
            unsigned b_above = b >> BITS & MASK;

            // Edge rules are implemented here
            //  * Type 0 is symmetric (e.g. flat) and therefore only matches itself
            //  * Type 1&2 are asymmetric (e.g. male-female) and always come in pair
            right[a][b] = (a_right == 0 && b_left == 0)
                       || (a_right > 0 && a_right % 2 == 0 && b_left == a_right - 1)
                       || (a_right > 0 && a_right % 2 == 1 && b_left == a_right + 1);
            below[a][b] = (a_below == 0 && b_above == 0)
                       || (a_below > 0 && a_below % 2 == 0 && b_above == a_below - 1)
                       || (a_below > 0 && a_below % 2 == 1 && b_above == a_below + 1);
        }

    //
    // PIECE SET
    //

    // We have one of each (canonical, i.e. unique) piece
    bool has[NUM] = {};
    for (unsigned p = 0; p < NUM; ++p)
        has[canonical[p]] = true;

    // We do not want pieces with any rotational symmetry
    for (unsigned i = 0; i < TYPES; ++i)
        for (unsigned j = 0; j < TYPES; ++j)
            has[i | j << BITS | i << (2 * BITS) | j << (3 * BITS)] = false;

    // Some edge types do not exist
    // Note: depending on the configuration, there may be a lot of non-existent pieces!
    for (unsigned p = 0; p < NUM; ++p)
        for (int i = 0; i < 4; ++i)
            if ((p >> (BITS * i) & MASK) >= TYPES)
                has[p] = false;

    // Drop unwanted edge pieces (i.e. the ones with 3 flat edges) 
    for (unsigned i = 0; i < TYPES; ++i)
        has[canonical[i]] = false;

    // Drop some pieces randomly
    // Note: this is needed to solve in an acceptable amount of time, but this is tricky to fine-tune...
    //  * Too few pieces means no solution
    //  * Too many pieces means an insane amount of time
    srand(1337);
    for (unsigned p = 0; p < NUM; ++p)
        if (has[p])
            has[p] = rand() < RAND_MAX / 4;

    // Print available pieces
    unsigned count = 0;
    for (unsigned p = 0; p < NUM; ++p)
        if (has[p]) {
            ++count;
            fprintf(stderr, "%02u) %u, %u, %u, %u -> %u\n", count, p & MASK, p >> BITS & MASK, p >> (2 * BITS) & MASK, p >> (3 * BITS) & MASK, p);
        }

    // Do the search
    // Note: one could also print solutions directly, but there are usually too many of them
    auto start = std::chrono::high_resolution_clock::now();
    //search(has, print);
    search(has, collect);
    auto end = std::chrono::high_resolution_clock::now();
    fprintf(stderr, "\n");

    // Done
    std::chrono::duration<float> duration = end - start;
    fprintf(stderr, "%llu solutions, grouped in %zu sets, in %.03f seconds.\n", total, counter.size(), duration.count());

    // Report only the solutions that have the smallest number of combination
    // Note: a jigsaw with unique solution usually has 4 of them, given that the whole board can be rotated
    if (counter.size() > 0) {
        uint64_t minimum = -1;
        for (auto & it : counter) {
            minimum = std::min(minimum, it.second);
        }
        fprintf(stderr, "Best selections have %llu solutions\n", minimum);
        for (auto & it : counter) 
            if (it.second == minimum)
                print(it.first.stack);
    }

    return 0;
}
