
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <map>


constexpr unsigned WIDTH = 4;
constexpr unsigned HEIGHT = 4;

/*
constexpr unsigned BITS = 2;
constexpr unsigned MASK = (1 << BITS) - 1;
constexpr unsigned NUM = (1 << BITS) * (1 << BITS) * (1 << BITS) * (1 << BITS);
constexpr unsigned TYPES = 3;
typedef uint8_t Piece;
*/

constexpr unsigned BITS = 3;
constexpr unsigned MASK = (1 << BITS) - 1;
constexpr unsigned NUM = (1 << BITS) * (1 << BITS) * (1 << BITS) * (1 << BITS);
constexpr unsigned TYPES = 5;
typedef uint16_t Piece;


Piece canonical[NUM];
bool right[NUM][NUM];
bool below[NUM][NUM];



struct Solution {
    Piece stack[WIDTH * HEIGHT];
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




template <typename Callback>
void search(bool has[], Callback & callback) {

    // Recurse, using stack for efficiency
    uint64_t total = 0;
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
        if (x == 0) {
            if ((piece >> (2 * BITS) & MASK) != 0)
                fit = false;
        } else {
            if (x == WIDTH - 1) {
                if ((piece & MASK) != 0)
                    fit = false;
            }
            if (!right[stack[index - 1]][piece])
                fit = false;
        }
        if (y == 0) {
            if ((piece >> BITS & MASK) != 0)
                fit = false;
        } else {
            if (y == HEIGHT - 1) {
                if ((piece >> (3 * BITS) & MASK) != 0)
                    fit = false;
            }
            if (!below[stack[index - WIDTH]][piece])
                fit = false;
        }
        if (fit) {

            // Is jigsaw complete?
            if (index == WIDTH * HEIGHT - 1) {
                if (!callback(stack))
                    return;
                goto inc;
            }
            has[canonical[stack[index]]] = false;
            ++index;
            stack[index] = 0;
            goto check;
        }
    }

    // Increment current depth
inc:
    if (stack[index] == NUM - 1) {
        if (index == 0)
            goto done;
        --index;
        has[canonical[stack[index]]] = true;
        goto inc;
    }
    if (index == 0)
        putc('.', stderr);
    ++stack[index];
    goto check;

    // Search complete
done:
    ;
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


std::map<Solution, uint64_t> counter;

bool collect(Piece stack[]) {
    static uint64_t k = 0;
    Solution solution(stack);
    ++counter[solution];
    /*
    if (++k % 100000 == 0 || true) {
        fprintf(stderr, "%llu (%zu)\n", k, counter.size());
        for (unsigned i = 0; i < WIDTH; ++i)
            fprintf(stderr, "  %u", solution.stack[i]);
        fprintf(stderr, "\n");
    }*/
    return true;
}


int main(int argc, char* argv[]) {

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
            right[a][b] = (a_right == 0 && b_left == 0) || (a_right > 0 && a_right % 2 == 0 && b_left == a_right - 1) || (a_right > 0 && a_right % 2 == 1 && b_left == a_right + 1);
            unsigned a_below = a >> (3 * BITS) & MASK;
            unsigned b_above = b >> BITS & MASK;
            below[a][b] = (a_below == 0 && b_above == 0) || (a_below > 0 && a_below % 2 == 0 && b_above == a_below - 1) || (a_below > 0 && a_below % 2 == 1 && b_above == a_below + 1);
        }

    // We have one of each (canonical, i.e. unique) piece
    bool has[NUM] = {};
    for (unsigned p = 0; p < NUM; ++p)
        has[canonical[p]] = true;

    // We do not want pieces with any rotational symmetry
    for (unsigned i = 0; i < TYPES; ++i)
        for (unsigned j = 0; j < TYPES; ++j)
            has[i | j << BITS | i << (2 * BITS) | j << (3 * BITS)] = false;

    // Some edge types do not exist
    for (unsigned p = 0; p < NUM; ++p)
        for (int i = 0; i < 4; ++i)
            if ((p >> (BITS * i) & MASK) >= TYPES)
                has[p] = false;

    // Drop some pieces randomly
    srand(1337);
    for (unsigned p = 0; p < NUM; ++p)
        if (has[p])
            has[p] = rand() < RAND_MAX / 5;

    // Print available pieces
    unsigned count = 0;
    for (unsigned p = 0; p < NUM; ++p)
        if (has[p]) {
            ++count;
            fprintf(stderr, "%02u) %u, %u, %u, %u -> %u\n", count, p & MASK, p >> BITS & MASK, p >> (2 * BITS) & MASK, p >> (3 * BITS) & MASK, p);
        }

    //search(has, print);
    search(has, collect);

    fprintf(stderr, "%zu selections.\n", counter.size());

    uint64_t minimum = -1;
    for (auto & it : counter) {
        minimum = std::min(minimum, it.second);
    }

    fprintf(stderr, "Best selections have %llu solutions\n", minimum);
    for (auto & it : counter) 
        if (it.second == minimum) {
            //printf("%llu ", it.second);
            print(it.first.stack);
        }

    return 0;
}

