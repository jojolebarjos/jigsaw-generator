
// g++ -O3 -std=c++17 -o inner_thread inner_thread.cpp


#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <random>
#include <thread>


#define WIDTH 4
#define HEIGHT 4

#define MAX_SOLUTIONS 16


constexpr uint8_t flip(uint8_t p) {
    return p >> 6 | (p >> 2) & 0b1100 | (p << 2) & 0b110000 | p << 6;
}


constexpr uint8_t rotate(uint8_t p) {
    return p >> 2 | p << 6;
}


constexpr uint8_t to_canonical(uint8_t p) {
    uint8_t ps[8] = {};
    ps[0] = p;
    ps[1] = rotate(ps[0]);
    ps[2] = rotate(ps[1]);
    ps[3] = rotate(ps[2]);
    ps[4] = flip(p);
    ps[5] = rotate(ps[4]);
    ps[6] = rotate(ps[5]);
    ps[7] = rotate(ps[6]);
    return *std::min_element(ps, ps + 8);
}


constexpr auto make_canonical() {
    std::array<uint8_t, 256> result = {};
    for (unsigned p = 0; p < 256; ++p)
        result[p] = to_canonical(p);
    return result;
}


constexpr auto canonical = make_canonical();


constexpr bool is_match(unsigned a, unsigned b) {
    bool type_match = (a & 0b10) == (b & 0b10);
    bool gender_match = (a & 0b01) != (b & 0b01);
    return type_match && gender_match;
}


constexpr bool is_horizontal_match(uint8_t a, uint8_t b) {
    uint8_t a_right = a & 0b11;
    uint8_t b_left = (b >> 4) & 0b11;
    return is_match(a_right, b_left);
}


constexpr bool is_vertical_match(uint8_t a, uint8_t b) {
    uint8_t a_below = a >> 6;
    uint8_t b_above = (b >> 2) & 0b11;
    return is_match(a_below, b_above);
}


int64_t search(std::array<uint8_t, 256> has) {

    // Recurse, using stack for efficiency
    uint8_t stack[HEIGHT * WIDTH];
    unsigned index = 0;
    stack[0] = 0;
    int64_t counter = 0;

    // Check if we have this piece
check:
    if (has[canonical[stack[index]]] > 0) {

        // Check that this piece is allowed there
        uint8_t piece = stack[index];
        bool fit = true;

        // Edge must match piece on the left side
        if ((index % WIDTH) > 0 && !is_horizontal_match(stack[index - 1], piece))
            fit = false;

        // Edge must match piece above
        if (index >= WIDTH && !is_vertical_match(stack[index - WIDTH], piece))
            fit = false;

        // We can place it
        if (fit) {

            // Is jigsaw complete?
            if (index == WIDTH * HEIGHT - 1) {
                counter += 1;
                if (counter > 8 * MAX_SOLUTIONS)
                    return -1;
                goto inc;
            }

            // Otherwise, push piece on stack
            has[canonical[stack[index]]] -= 1;
            ++index;
            stack[index] = 0;
            goto check;
        }
    }

    // Check whether this depth is exhausted
inc:
    if (stack[index] == 255) {

        // That was the highest level, search is therefore complete
        if (index == 0)
            return counter;

        // Pop from stack
        --index;
        has[canonical[stack[index]]] += 1;
        goto inc;
    }

    // Increment piece
    ++stack[index];
    goto check;
}


constexpr unsigned num_threads = 16;


void run(unsigned id) {

    // Seed generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 10000000);

    // Loop until we find something
    while (true) {

        // Using "dumb" generator, so that we can reproduce easily using a seed
        int seed = distribution(generator) * num_threads + id;
        std::mt19937 g(seed);
        std::uniform_int_distribution<unsigned> d(0, 3);

        // Generate random edges
        uint8_t left[HEIGHT][WIDTH + 1] = {};
        for (unsigned y = 0; y < HEIGHT; ++y)
            for (unsigned x = 0; x <= WIDTH; ++x)
                left[y][x] = d(g);
        uint8_t up[HEIGHT + 1][WIDTH] = {};
        for (unsigned y = 0; y <= HEIGHT; ++y)
            for (unsigned x = 0; x < WIDTH; ++x)
                up[y][x] = d(g);

        // Create pieces
        uint8_t grid[HEIGHT * WIDTH] = {};
        for (unsigned y = 0; y < HEIGHT; ++y)
            for (unsigned x = 0; x < WIDTH; ++x)
                grid[y * WIDTH + x] = (left[y][x + 1] ^ 0b01)
                                    | up[y][x] << 2
                                    | left[y][x] << 4
                                    | (up[y + 1][x] ^ 0b01) << 6;

        // Count pieces
        std::array<uint8_t, 256> has = {};
        for (unsigned i = 0; i < HEIGHT * WIDTH; ++i)
            has[canonical[grid[i]]] += 1;

        // Search
        int64_t count = search(has);
        if (count < 0)
            continue;

        // Print solution
        printf("%u... OK, %lu solutions!\n", seed, count);
        for (unsigned y = 0; y < HEIGHT; ++y) {
            for (unsigned x = 0; x < WIDTH; ++x)
                printf("% 4u", grid[y * WIDTH + x]);
            printf("\n");
        }

        // We are done
        std::exit(1);
        return;
    }
}


int main(int argc, char* argv[]) {
    std::thread threads[num_threads];
    for (unsigned i = 0; i < num_threads; ++i)
        threads[i] = std::thread(run, i);
    for (unsigned i = 0; i < num_threads; ++i)
        threads[i].join();
    return 0;
}
