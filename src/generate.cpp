
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <mutex>
#include <thread>

#include "piece.h"
#include "problem.h"
#include "grid.h"


std::mutex lock;
bool last_dot = true;


void run(unsigned id) {

    // Random device
    // Note: using thread index to make sure nobody has the same seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed += 1337 * id;
    std::default_random_engine generator(seed);

    // Distribution over desired edge types
    unsigned edges[] = {
        //TYPE_CENTERED_BIG_MALE,
        //TYPE_CENTERED_BIG_FEMALE,
        TYPE_CENTERED_MEDIUM_MALE,
        TYPE_CENTERED_MEDIUM_FEMALE,
        TYPE_CENTERED_SMALL_MALE,
        TYPE_CENTERED_SMALL_FEMALE,
        //TYPE_RIGHT_SMALL_MALE,
        //TYPE_RIGHT_SMALL_FEMALE,
        //TYPE_LEFT_SMALL_MALE,
        //TYPE_LEFT_SMALL_FEMALE,
        //TYPE_DOUBLE_SMALL_MALE,
        //TYPE_DOUBLE_SMALL_FEMALE,
        //TYPE_TWISTED_SMALL_MALE,
        //TYPE_TWISTED_SMALL_FEMALE,
    };
    std::uniform_int_distribution<unsigned> distribution(0, sizeof(edges) / sizeof(edges[0]) - 1);
    auto sampler = [&]() { return edges[distribution(generator)]; };

    // Loop forever
    while (true) {

        // Generate random board, where all pieces are unique
        Grid grid(5, 5);
        do {
            grid.randomize(sampler);
            grid.horizontal[1 * 6 + 2] = 0;
            grid.vertical[2 * 5 + 2] = 0;
        } while (grid.has_duplicate());

        // Create solver
        Definition definition = grid.to_definition();
        Problem problem(definition);

        // Count solutions
        // Note: by design, more than 8 solutions means that it is not unique
        unsigned count = 0;
        while (count < 9 && problem.next()) {
            ++count;
        }

        // Show a dot on failure, as progress indication
        if (count > 8) {
            std::lock_guard<std::mutex> guard(lock);
            putc('.', stdout);
            last_dot = true;
            continue;
        }

        // Report
        {
            std::lock_guard<std::mutex> guard(lock);
            if (last_dot) {
                last_dot = false;
                putc('\n', stdout);
            }
            printf("%u ", count);
            grid.print();
        }
    }
}


constexpr unsigned num_threads = 16;

int main(int argc, char* argv[]) {
    if (num_threads <= 1)
        run(0);
    else {
        std::thread threads[num_threads];
        for (unsigned i = 0; i < num_threads; ++i)
            threads[i] = std::thread(run, i);
        for (unsigned i = 0; i < num_threads; ++i)
            threads[i].join();
    }
    return 0;
}
