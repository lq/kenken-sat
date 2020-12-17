#include "cage.hpp"
#include "solver.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace {


/**
 * Input patterns:
 * First line: The number of squares in each row and column
 * Each following line specifies a cage with the format:
 *  result operator {row column}..
 *
 * Example:
 *
 *  3
 *  2/ 00 10
 *  2- 01 02
 *  2* 11 21
 *  5+ 12 22
 *  3= 20
 *
 */
board init_board(std::istream& in) {
    in >> board_size;
    board parsed(board_size, std::vector<int>(board_size));
    return parsed;
}

std::vector<Cage> read_cages(std::istream& in)
{
    std::vector<Cage> cages;
    Cage cage;
    while (in >> cage)
        cages.emplace_back(cage);
    return cages;
}
} // end anonymous namespace


int main() {
    try {
        auto board = init_board(std::cin);
        auto cages = read_cages(std::cin);
        auto t1 = std::chrono::steady_clock::now();
        Solver s{true};
        for (auto& cage : cages) {
            if (!s.apply_cage(cage)) {
                std::clog << "There is a contradiction in the parsed!\n";
                return 2;
            }
        }
        if (s.solve()) {
            std::chrono::duration<double, std::milli> time_taken = std::chrono::steady_clock::now() - t1;
            std::clog << "Solution found in " << time_taken.count() << " ms\n";

            auto solution = s.get_solution();
            for (auto const& row : solution) {
                for (auto const& col : row) {
                    std::cout << col << ' ';
                }
                std::cout << '\n';
            }
        } else {
            std::clog << "Solving the provided parsed is not possible\n";
        }
    } catch(std::exception const& ex) {
        std::clog << "Failed parsing because: " << ex.what() << std::endl;
        return 1;
    }
}
