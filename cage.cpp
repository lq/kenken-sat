//
// Created by Li Qiang on 2020/12/16.
//

#include "cage.hpp"
#include <istream>
#include <stdexcept>

void Cage::find_combinations()
{
    buffer.resize(size(coord));
    find_comb_util(0, 1, result);
}

void Cage::find_comb_util(int idx, int min, int rem)
{
    if (idx == size(buffer) - 1) {
        if (rem <= board_size) {
            buffer[idx] = rem - 1;
            combinations.insert(end(combinations), begin(buffer), end(buffer));
        }
        return;
    }
    for (int i = min; i <= board_size; ++i) {
        auto r = rem;
        switch (op) {
        case Op::add:
            r -= i;
            break;
        case Op::minus:
            r += i;
            break;
        case Op::multiply:
            if (r % i != 0)
                continue;
            r /= i;
            break;
        case Op::divide:
            r *= i;
            break;
        default:
            throw std::invalid_argument{"Impossible"};
        }
        if (r >= i) {
            buffer[idx] = i - 1;
            find_comb_util(idx + 1, i, r);
        } else return;
    }
}

std::istream& operator>>(std::istream& is, Cage& x)
{
    std::istream::sentry s{is};
    if (!s) {
        is.setstate(std::ios::failbit);
        return is;
    }

    is >> x.result;

    char c;
    is >> c;
    x.op = static_cast<Op>(c);

    x.coord.clear();
    char c2;
    while (is.peek() != '\n') {
        is >> c >> c2;
        x.coord.push_back({c - '0', c2 - '0'});
    }

    return is;
}
