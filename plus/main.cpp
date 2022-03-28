
// set PATH=D:\Projects\mingw\mingw64\bin;%PATH%
// g++ -g -O0 -std=c++17 -o main main.cpp


#include <cstdio>
#include <cstdint>

#include "piece.h"
#include "problem.h"
#include "grid.h"


int main(int argc, char* argv[]) {

    
    Definition definition(5, 5);
    definition.pieces[{3, 0, 0, 4}] += 1;
    definition.pieces[{3, 0, 4, 2}] += 1;
    definition.pieces[{4, 0, 4, 4}] += 1;
    definition.pieces[{4, 0, 3, 1}] += 1;
    definition.pieces[{0, 0, 3, 1}] += 1;
    definition.pieces[{3, 3, 0, 4}] += 1;
    definition.pieces[{0, 1, 4, 3}] += 1;
    definition.pieces[{3, 3, 0, 0}] += 1;
    definition.pieces[{2, 2, 4, 2}] += 1;
    definition.pieces[{0, 2, 1, 1}] += 1;
    definition.pieces[{3, 3, 0, 2}] += 1;
    definition.pieces[{4, 4, 4, 3}] += 1;
    definition.pieces[{3, 0, 3, 1}] += 1;
    definition.pieces[{2, 1, 4, 4}] += 1;
    definition.pieces[{0, 2, 1, 4}] += 1;
    definition.pieces[{1, 1, 0, 1}] += 1;
    definition.pieces[{2, 4, 2, 3}] += 1;
    definition.pieces[{4, 2, 1, 1}] += 1;
    definition.pieces[{1, 3, 3, 4}] += 1;
    definition.pieces[{0, 3, 2, 2}] += 1;
    definition.pieces[{2, 2, 0, 0}] += 1;
    definition.pieces[{1, 4, 1, 0}] += 1;
    definition.pieces[{1, 2, 2, 0}] += 1;
    definition.pieces[{2, 3, 2, 0}] += 1;
    definition.pieces[{0, 1, 1, 0}] += 1;
    
    Problem problem(definition);
    
    while (problem.next()) {
        Grid grid = Grid::from_problem(problem);
        grid.print();
    }
    printf("done\n");
    

/*
    std::default_random_engine generator;

    Grid grid(5, 5);

    unsigned edges[] = {1, 2, 3, 4};
    std::uniform_int_distribution<unsigned> distribution(0, sizeof(edges) / sizeof(edges[0]));
    auto sampler = [&]() { return edges[distribution(generator)]; };

    std::map<Grid, unsigned> count;
    uint64_t total_count = 0;
    for (unsigned i = 0; i < 100000; ++i) {
        while (grid.has_duplicate()) {
            total_count++;
            grid.randomize(sampler);
        }
        //grid.print();
        //grid.print_canonical();
        
        count[grid.canonical()] += 1;
        
        grid.randomize(sampler);
        total_count++;
    }
    
    printf("%llu (%.02f%%)\n", total_count, total_count / 100000);

    std::map<unsigned, unsigned> k;
    for (auto const & pair : count)
        k[pair.second] += 1;
    
    for (auto const & pair : k)
        printf("%u: %u\n", pair.first, pair.second);
    

    std::map<std::vector<Piece>, unsigned> q;
    for (auto const & pair : count) {
        std::vector<Piece> ps = pair.first.pieces();
        for (Piece & p : ps)
            p = p.canonical();
        std::sort(ps.begin(), ps.end());
        q[ps] += pair.second;
    }
    
    std::map<unsigned, unsigned> qk;
    for (auto const & pair : q)
        qk[pair.second] += 1;
    
    for (auto const & pair : qk)
        printf("%u: %u\n", pair.first, pair.second);

*/

    return 0;
}
