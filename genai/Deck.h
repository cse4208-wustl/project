#pragma once
// A generic deck of cards plus the three concrete decks used by the games.

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "Card.h"
#include "Util.h"

template <typename R, typename S>
class Deck {
public:
    using CardType = Card<R, S>;
    using RankType = R;
    using SuitType = S;

    void shuffle() { std::shuffle(cards_.begin(), cards_.end(), rng()); }
    bool empty() const { return cards_.empty(); }
    std::size_t size() const { return cards_.size(); }

    // Removes and returns the top card.
    CardType draw() {
        if (cards_.empty()) throw std::runtime_error("cannot draw from an empty deck");
        CardType top = cards_.back();
        cards_.pop_back();
        return top;
    }

protected:
    std::vector<CardType> cards_;
};

// Standard 52-card deck.
class HoldEmDeck : public Deck<HoldEmRank, Suit> {
public:
    static constexpr const char* name = "Texas Hold'em deck (52 cards)";
    HoldEmDeck() {
        for (Suit s : allSuits)
            for (HoldEmRank r : allHoldEmRanks) cards_.emplace_back(r, s);
    }
};

// Pinochle deck: two copies each of 9, J, Q, K, 10, A in four suits (48 cards).
class PinochleDeck : public Deck<PinochleRank, Suit> {
public:
    static constexpr const char* name = "Pinochle deck (48 cards)";
    PinochleDeck() {
        for (int copy = 0; copy < 2; ++copy)
            for (Suit s : allSuits)
                for (PinochleRank r : allPinochleRanks) cards_.emplace_back(r, s);
    }
};

// Uno deck (108 cards): per color one 0 and two each of 1-9, Skip, Reverse and
// Draw Two; plus four Wild and four Wild Draw Four cards.
class UnoDeck : public Deck<UnoRank, UnoColor> {
public:
    static constexpr const char* name = "Uno deck (108 cards)";
    UnoDeck() {
        for (UnoColor c : unoMainColors) {
            cards_.emplace_back(UnoRank::zero, c);
            for (int r = static_cast<int>(UnoRank::one); r <= static_cast<int>(UnoRank::drawTwo); ++r) {
                cards_.emplace_back(static_cast<UnoRank>(r), c);
                cards_.emplace_back(static_cast<UnoRank>(r), c);
            }
        }
        for (int i = 0; i < 4; ++i) {
            cards_.emplace_back(UnoRank::wild, UnoColor::black);
            cards_.emplace_back(UnoRank::wildDrawFour, UnoColor::black);
        }
    }
};
