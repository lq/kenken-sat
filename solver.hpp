#pragma once

#include "cage.hpp"
#include <minisat/core/Solver.h>
#include <vector>

extern int board_size;
using board = std::vector<std::vector<int>>;

class Solver {
private:
    const bool m_write_dimacs = false;
    Minisat::Solver solver;

public:
    Solver(bool write_dimacs = false);
    bool apply_cage(Cage& cage);
    // Returns true if the sudoku has a solution
    bool solve();
    board get_solution() const;

private:
    void one_square_one_value();
    void non_duplicated_values();
    void exactly_one_true(Minisat::vec<Minisat::Lit> const& literals);
    void init_variables();

    bool apply_equal(const Cage&);
    bool apply_combinations(Cage& cage);
};
