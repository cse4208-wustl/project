#pragma once
// A playing card parameterized by its rank and suit types.

#include <ostream>
#include <sstream>
#include <string>

#include "CardTypes.h"

template <typename R, typename S>
struct Card {
    using RankType = R;
    using SuitType = S;

    R rank;
    S suit;

    Card(R r, S s) : rank(r), suit(s) {}
};

template <typename R, typename S>
bool operator==(const Card<R, S>& a, const Card<R, S>& b) {
    return a.rank == b.rank && a.suit == b.suit;
}

// Orders by rank first, then by suit.
template <typename R, typename S>
bool operator<(const Card<R, S>& a, const Card<R, S>& b) {
    return a.rank < b.rank || (a.rank == b.rank && a.suit < b.suit);
}

// Default card text is rank followed by suit, e.g. "KS" or "10H".
template <typename R, typename S>
std::ostream& operator<<(std::ostream& os, const Card<R, S>& c) {
    return os << c.rank << c.suit;
}

// Uno cards read better as "red 7" or "Wild Draw Four".
std::ostream& operator<<(std::ostream& os, const Card<UnoRank, UnoColor>& c);

// Converts anything printable to a string.
template <typename T>
std::string toString(const T& x) {
    std::ostringstream out;
    out << x;
    return out.str();
}
