
#include <cstdio>
#include <cstdint>


#define WIDTH 3
#define HEIGHT 3


uint8_t canonical[256];
bool right[256][256];
bool below[256][256];


template <typename Callback>
void search(bool has[], Callback & callback) {

    // Recurse, using stack for efficiency
    uint64_t total = 0;
    uint8_t stack[WIDTH * HEIGHT];
    unsigned index = 0;
    stack[0] = 0;

    // Check if we have this piece
check:
    if (has[canonical[stack[index]]]) {

        // Check that this piece is allowed there
        uint8_t piece = stack[index];
        bool fit = true;
        unsigned x = index % WIDTH;
        unsigned y = index / WIDTH;
        if (x == 0) {
            if ((piece >> 4 & 0b11) != 0)
                fit = false;
        } else {
            if (x == WIDTH - 1) {
                if ((piece & 0b11) != 0)
                    fit = false;
            }
            if (!right[stack[index - 1]][piece])
                fit = false;
        }
        if (y == 0) {
            if ((piece >> 2 & 0b11) != 0)
                fit = false;
        } else {
            if (y == HEIGHT - 1) {
                if ((piece >> 6 & 0b11) != 0)
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
    if (stack[index] == 255) {
        if (index == 0)
            goto done;
        --index;
        has[canonical[stack[index]]] = true;
        goto inc;
    }
    //if (index == 0)
        //putc('.', stderr);
    ++stack[index];
    goto check;

    // Search complete
done:
    ;
}


bool print(uint8_t stack[]) {
    putchar('[');
    for (unsigned i = 0; i < WIDTH * HEIGHT; ++i) {
        if (i > 0)
            putchar(',');
        uint8_t p = stack[i];
        printf("%u,%u,%u,%u", p & 0b11, p >> 2 & 0b11, p >> 4 & 0b11, p >> 6 & 0b11);
    }
    printf("]\n");
    return true;
}


int main(int argc, char* argv[]) {

    // Generate mapping to unique pieces
    for (unsigned p = 0; p < 256; ++p) {
        canonical[p] = p;
        uint8_t q = p;
        for (int i = 0; i < 4; ++i) {
            q = q >> 2 | q << 6;
            if (q < p)
                break;
        }
        canonical[p] = canonical[q];
    }

    // Whether two pieces can be together
    for (unsigned a = 0; a < 256; ++a)
        for (unsigned b = 0; b < 256; ++b) {
            unsigned a_right = a & 0b11;
            unsigned b_left = b >> 4 & 0b11;
            right[a][b] = (a_right == 0 && b_left == 0) || (a_right == 1 && b_left == 2) || (a_right == 2 && b_left == 1);
            unsigned a_below = a >> 6 & 0b11;
            unsigned b_above = b >> 2 & 0b11;
            below[a][b] = (a_below == 0 && b_above == 0) || (a_below == 1 && b_above == 2) || (a_below == 2 && b_above == 1);
        }

    // We have one of each (canonical, i.e. unique) piece
    bool has[256] = {};
    for (unsigned p = 0; p < 256; ++p)
        has[canonical[p]] = true;

    // We do not want pieces with any rotational symmetry
    for (unsigned i = 0; i < 4; ++i)
        for (unsigned j = 0; j < 4; ++j)
            has[i | j << 2 | i << 4 | j << 6] = false;

    // Edge type 4 does not exist
    for (unsigned p = 0; p < 256; ++p)
        for (int i = 0; i < 4; ++i)
            if ((p >> (2 * i) & 0b11) == 3)
                has[p] = false;
/*
    // Force piece set
    if (false) {
        uint8_t solution[WIDTH * HEIGHT] = {
            0x80, 0x41, 0x21, 0x60,
            0x45, 0x69, 0xa2, 0x98,
            0x4a, 0x9a, 0x56, 0x94,
            0x0a, 0x16, 0x19, 0x24,
        };
        for (unsigned p = 0; p < 256; ++p)
            has[p] = false;
        for (unsigned i = 0; i < WIDTH * HEIGHT; ++i)
            has[canonical[solution[i]]] = true;
    }
*/

    /*
    unsigned count = 0;
    for (unsigned p = 0; p < 256; ++p)
        if (has[p]) {
            ++count;
            printf("%02u) %u, %u, %u, %u -> %u\n", count, p & 0b11, p >> 2 & 0b11, p >> 4 & 0b11, p >> 6 & 0b11, p);
        }
    return 0;
    */

    search(has, print);

    return 0;
}

