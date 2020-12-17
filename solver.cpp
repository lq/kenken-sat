#include "solver.hpp"

#include <algorithm>    // next_permutation
#include <iostream>
#include <cassert>

int board_size;

namespace {

Minisat::Var toVar(int row, int column, int value) {
    assert(row >= 0 && row < board_size && "Attempt to get var for nonexistant row");
    assert(column >= 0 && column < board_size && "Attempt to get var for nonexistant column");
    assert(value >= 0 && value < board_size && "Attempt to get var for nonexistant value");
    return row * board_size * board_size + column * board_size + value;
}

void log_var(Minisat::Lit lit) {
    if (sign(lit)) {
        std::clog << '-';
    }
    std::clog << var(lit) + 1 << ' ';
}

void log_clause(Minisat::vec<Minisat::Lit> const& clause) {
    for (int i = 0; i < clause.size(); ++i) {
        log_var(clause[i]);
    }
    std::clog << "0\n";
}

void log_clause(Minisat::Lit lhs, Minisat::Lit rhs) {
    log_var(lhs); log_var(rhs);
    std::clog << "0\n";
}


} //end anonymous namespace

void Solver::init_variables() {
    if (m_write_dimacs) {
        std::clog << "c (row, column, value) = variable\n";
    }
    for (int r = 0; r < board_size; ++r) {
        for (int c = 0; c < board_size; ++c) {
            for (int v = 0; v < board_size; ++v) {
                auto var = solver.newVar();
                if (m_write_dimacs) {
                    std::clog << "c (" << r << ", " << c << ", " << v + 1 << ") = " << var + 1 << '\n';
                }
            }
        }
    }
    std::clog << std::flush;
}


void Solver::exactly_one_true(Minisat::vec<Minisat::Lit> const& literals) {
    if (m_write_dimacs) {
        log_clause(literals);
    }
    solver.addClause(literals);
    for (size_t i = 0; i < literals.size(); ++i) {
        for (size_t j = i + 1; j < literals.size(); ++j) {
            if (m_write_dimacs) {
                log_clause(~literals[i], ~literals[j]);
            }
            solver.addClause(~literals[i], ~literals[j]);
        }
    }
}



void Solver::one_square_one_value() {
    for (int row = 0; row < board_size; ++row) {
        for (int column = 0; column < board_size; ++column) {
            Minisat::vec<Minisat::Lit> literals;
            for (int value = 0; value < board_size; ++value) {
                literals.push(Minisat::mkLit(toVar(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
}

void Solver::non_duplicated_values() {
    // In each row, for each value, forbid two column sharing that value
    for (int row = 0; row < board_size; ++row) {
        for (int value = 0; value < board_size; ++value) {
            Minisat::vec<Minisat::Lit> literals;
            for (int column = 0; column < board_size; ++column) {
                literals.push(Minisat::mkLit(toVar(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
    // In each column, for each value, forbid two board_size sharing that value
    for (int column = 0; column < board_size; ++column) {
        for (int value = 0; value < board_size; ++value) {
            Minisat::vec<Minisat::Lit> literals;
            for (int row = 0; row < board_size; ++row) {
                literals.push(Minisat::mkLit(toVar(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
}

Solver::Solver(bool write_dimacs):
    m_write_dimacs(write_dimacs) {
    // Initialize the board
    init_variables();
    one_square_one_value();
    non_duplicated_values();
}

bool Solver::solve() {
    return solver.solve();
}

board Solver::get_solution() const {
    board b(board_size, std::vector<int>(board_size));
    for (int row = 0; row < board_size; ++row) {
        for (int col = 0; col < board_size; ++col) {
            int found = 0;
            for (int val = 0; val < board_size; ++val) {
                if (solver.modelValue(toVar(row, col, val)).isTrue()) {
                    ++found;
                    b[row][col] = val + 1;
                }
            }
            assert(found == 1 && "The SAT solver assigned one position more than one value");
            (void)found;
        }
    }
    return b;
}

bool Solver::apply_cage(Cage& cage)
{
    if (cage.op == Op::equal)
        return apply_equal(cage);
    cage.find_combinations();
    return apply_combinations(cage);
}

bool Solver::apply_equal(const Cage& cage)
{
    assert(size(cage.coord) == 1);
    auto var = toVar(cage.coord[0][0], cage.coord[0][1], cage.result - 1);
    if (m_write_dimacs) {
        log_var(Minisat::mkLit(var));
        std::clog << "0\n";
    }
    return solver.addClause(Minisat::mkLit(var));
}


bool Solver::apply_combinations(Cage& cage)
{
    auto res = true;
    Minisat::vec<Minisat::Lit> lit_patterns;
    const auto n = size(cage.coord);
    for (auto comb = begin(cage.combinations); comb != end(cage.combinations); comb += n) {
        do {
            auto lit_patt = Minisat::mkLit(solver.newVar());
            for (int i = 0; i < n && res; ++i) {
                auto [r, c] = cage.coord[i];
                auto lit_square = Minisat::mkLit(toVar(r, c, comb[i]));
                if (m_write_dimacs)
                    log_clause(~lit_patt, lit_square);
                res = solver.addClause(~lit_patt, lit_square);
            }
            lit_patterns.push(lit_patt);
        } while (std::next_permutation(comb, comb + n));
    }
    if (res) {
        if (m_write_dimacs)
            log_clause(lit_patterns);
        res = solver.addClause(lit_patterns);
    }
    return res;
}