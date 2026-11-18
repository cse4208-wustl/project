#include "CardTypes.h"

#include <cstddef>

#include "Card.h"

namespace {
template <typename E>
std::size_t idx(E e) {
    return static_cast<std::size_t>(e);
}
}  // namespace

std::ostream& operator<<(std::ostream& os, Suit s) {
    static const char* const letters[] = {"C", "D", "H", "S"};
    return os << letters[idx(s)];
}

std::string suitName(Suit s) {
    static const char* const names[] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    return names[idx(s)];
}

std::ostream& operator<<(std::ostream& os, HoldEmRank r) {
    static const char* const names[] = {"2", "3", "4",  "5", "6", "7", "8",
                                        "9", "10", "J", "Q", "K", "A"};
    return os << names[idx(r)];
}

std::ostream& operator<<(std::ostream& os, PinochleRank r) {
    static const char* const names[] = {"9", "J", "Q", "K", "10", "A"};
    return os << names[idx(r)];
}

std::ostream& operator<<(std::ostream& os, UnoColor c) {
    static const char* const names[] = {"red", "yellow", "green", "blue", "black"};
    return os << names[idx(c)];
}

std::ostream& operator<<(std::ostream& os, UnoRank r) {
    static const char* const names[] = {"0", "1", "2", "3",       "4",       "5",    "6",
                                        "7", "8", "9", "Skip", "Reverse", "Draw Two",
                                        "Wild", "Wild Draw Four"};
    return os << names[idx(r)];
}

std::ostream& operator<<(std::ostream& os, const Card<UnoRank, UnoColor>& c) {
    if (c.suit == UnoColor::black) return os << c.rank;  // wild cards have no color
    return os << c.suit << ' ' << c.rank;
}
