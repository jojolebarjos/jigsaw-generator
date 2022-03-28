
// set PATH=D:\Projects\mingw\mingw64\bin;%PATH%
// g++ -O3 -std=c++17 -o generate generate.cpp


#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <map>
#include <random>
#include <set>
#include <vector>

#include "piece.h"
#include "problem.h"


int main(int argc, char* argv[]) {
    
    Definition definition(3, 3);

    definition.pieces[{4, 0, 0, 0}] = 1;
    definition.pieces[{4, 0, 3, 3}] = 1;
    definition.pieces[{0, 0, 3, 4}] = 1;
    definition.pieces[{4, 0, 0, 4}] = 1;
    definition.pieces[{4, 4, 3, 3}] = 1;
    definition.pieces[{0, 3, 3, 3}] = 1;
    definition.pieces[{3, 3, 0, 0}] = 1;
    definition.pieces[{4, 4, 4, 0}] = 1;
    definition.pieces[{0, 4, 3, 0}] = 1;
    
    Problem problem(definition);
    
    for (Piece const & p : problem.pieces)
        printf("%u, %u, %u, %u\n", p.edges[0], p.edges[1], p.edges[2], p.edges[3]);
    
    unsigned count = 0;
    while (problem.next()) {
        printf("[");
        for (unsigned y = 0; y < problem.H; ++y) {
            if (y > 0)
                printf(", ");
            printf("[");
            for (unsigned x = 0; x < problem.W; ++x) {
                if (x > 0)
                    printf(", ");
                Piece p = problem.pieces[problem.stack[(y + 1) * problem.W + x]];
                printf("[%u, %u, %u, %u]", p.edges[0], p.edges[1], p.edges[2], p.edges[3]);
            }
            printf("]");
        }
        printf("]\n");
    }
    
    return 0;
}


