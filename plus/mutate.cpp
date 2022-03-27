
// set PATH=D:\Projects\mingw\mingw64\bin;%PATH%
// g++ -g -O0 -std=c++17 -o mutate mutate.cpp


#include <cstdio>
#include <cstdint>
#include <functional>
#include <random>


#include "piece.h"
#include "grid.h"


int main(int argc, char* argv[]) {
    
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned> distribution(1, 4);
    auto sampler = std::bind(distribution, generator);

    Grid grid(5, 5);
    while (true) {
        while (grid.has_duplicate())
            grid.randomize(sampler);
        //grid.print();
        //grid.print_canonical();
        printf("OK\n");
        grid.randomize(sampler);
    }
    return 0;
}
