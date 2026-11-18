#pragma once
// Rank and suit types for the three kinds of decks used in this program:
// a standard 52-card (Texas Hold'em) deck, a 48-card Pinochle deck and a
// 108-card Uno deck.

#include <array>
#include <ostream>
#include <string>

// ---------- Suits (shared by the Texas Hold'em and Pinochle decks) ----------
enum class Suit { clubs, diamonds, hearts, spades };
inline constexpr std::array<Suit, 4> allSuits{Suit::clubs, Suit::diamonds, Suit::hearts,
                                              Suit::spades};
std::ostream& operator<<(std::ostream& os, Suit s);  // one letter: C D H S
std::string suitName(Suit s);                         // "Clubs", "Diamonds", ...

// ---------- Texas Hold'em ranks, lowest to highest ----------
enum class HoldEmRank { two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };
inline constexpr std::array<HoldEmRank, 13> allHoldEmRanks{
    HoldEmRank::two,  HoldEmRank::three, HoldEmRank::four,  HoldEmRank::five, HoldEmRank::six,
    HoldEmRank::seven, HoldEmRank::eight, HoldEmRank::nine, HoldEmRank::ten,  HoldEmRank::jack,
    HoldEmRank::queen, HoldEmRank::king, HoldEmRank::ace};
std::ostream& operator<<(std::ostream& os, HoldEmRank r);  // 2..10 J Q K A

// ---------- Pinochle ranks, lowest to highest (9 J Q K 10 A) ----------
enum class PinochleRank { nine, jack, queen, king, ten, ace };
inline constexpr std::array<PinochleRank, 6> allPinochleRanks{
    PinochleRank::nine, PinochleRank::jack, PinochleRank::queen,
    PinochleRank::king, PinochleRank::ten,  PinochleRank::ace};
std::ostream& operator<<(std::ostream& os, PinochleRank r);

// ---------- Uno ----------
enum class UnoColor { red, yellow, green, blue, black };
inline constexpr std::array<UnoColor, 4> unoMainColors{UnoColor::red, UnoColor::yellow,
                                                       UnoColor::green, UnoColor::blue};
std::ostream& operator<<(std::ostream& os, UnoColor c);

enum class UnoRank {
    zero, one, two, three, four, five, six, seven, eight, nine,
    skip, reverse, drawTwo, wild, wildDrawFour
};
std::ostream& operator<<(std::ostream& os, UnoRank r);
