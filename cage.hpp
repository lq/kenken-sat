//
// Created by Li Qiang on 2020/12/16.
//

#ifndef KENKEN_SAT_CAGE_HPP
#define KENKEN_SAT_CAGE_HPP

#include <array>
#include <iosfwd>
#include <vector>

extern int board_size;

enum class Op {
    add = '+', minus = '-', multiply = '*', divide = '/', equal = '='
};

class Cage {
public:
    void find_combinations();

    std::vector<std::array<int, 2>> coord;
    int result;
    Op op;
    std::vector<int> combinations;

private:
    void find_comb_util(int idx, int min, int rem);

    std::vector<int> buffer;
};

std::istream& operator>>(std::istream& is, Cage& x);

#endif//KENKEN_SAT_CAGE_HPP
