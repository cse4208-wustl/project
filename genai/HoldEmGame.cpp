#include "HoldEmGame.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Deck.h"
#include "Util.h"

namespace {

using HCard = Card<HoldEmRank, Suit>;
using Cards = std::vector<HCard>;

// Rank value used for comparing hands: 2..14 (ace high).
int value(HoldEmRank r) { return static_cast<int>(r) + 2; }

const char* const kRankWords[] = {"",    "",     "Two",  "Three", "Four",  "Five", "Six", "Seven",
                                  "Eight", "Nine", "Ten", "Jack",  "Queen", "King", "Ace"};
std::string word(int v) { return kRankWords[v]; }
std::string plural(int v) { return v == 6 ? "Sixes" : word(v) + "s"; }

enum Category {
    highCard, onePair, twoPair, threeOfAKind, straight, flush, fullHouse, fourOfAKind, straightFlush
};

// A hand's strength as a vector: {category, tie-breaker ranks...}.
// Two hands compare correctly with the vector's lexicographic operator<.
using HandKey = std::vector<int>;

HandKey evaluateFive(const Cards& hand) {
    std::map<int, int> counts;  // rank value -> number of cards of that rank
    bool isFlush = true;
    for (const HCard& c : hand) {
        ++counts[value(c.rank)];
        if (c.suit != hand[0].suit) isFlush = false;
    }

    // Groups sorted by size, then rank: e.g. full house K K K 7 7 -> (3,K) (2,7).
    std::vector<std::pair<int, int>> groups;  // (count, rank value)
    for (const auto& [v, n] : counts) groups.emplace_back(n, v);
    std::sort(groups.begin(), groups.end(), std::greater<>());

    bool isStraight = false;
    int high = 0;
    if (groups.size() == 5) {
        if (groups[0].second - groups[4].second == 4) {
            isStraight = true;
            high = groups[0].second;
        } else if (groups[0].second == 14 && groups[1].second == 5) {  // A-2-3-4-5 "wheel"
            isStraight = true;
            high = 5;
        }
    }

    int cat;
    if (isStraight && isFlush) cat = straightFlush;
    else if (groups[0].first == 4) cat = fourOfAKind;
    else if (groups[0].first == 3 && groups[1].first == 2) cat = fullHouse;
    else if (isFlush) cat = flush;
    else if (isStraight) cat = straight;
    else if (groups[0].first == 3) cat = threeOfAKind;
    else if (groups[0].first == 2 && groups[1].first == 2) cat = twoPair;
    else if (groups[0].first == 2) cat = onePair;
    else cat = highCard;

    HandKey key{cat};
    if (cat == straight || cat == straightFlush) {
        key.push_back(high);
    } else {
        for (const auto& g : groups) key.push_back(g.second);
    }
    return key;
}

std::string describe(const HandKey& k) {
    switch (k[0]) {
        case straightFlush:
            return k[1] == 14 ? "Royal flush" : "Straight flush, " + word(k[1]) + " high";
        case fourOfAKind: return "Four of a kind, " + plural(k[1]);
        case fullHouse: return "Full house, " + plural(k[1]) + " over " + plural(k[2]);
        case flush: return "Flush, " + word(k[1]) + " high";
        case straight: return "Straight, " + word(k[1]) + " high";
        case threeOfAKind: return "Three of a kind, " + plural(k[1]);
        case twoPair: return "Two pair, " + plural(k[1]) + " and " + plural(k[2]);
        case onePair: return "Pair of " + plural(k[1]);
        default: return "High card, " + word(k[1]);
    }
}

struct BestHand {
    HandKey key;
    Cards cards;
};

// Tries all 21 ways to choose 5 of the 7 available cards.
BestHand bestOfSeven(const Cards& seven) {
    BestHand best;
    for (std::size_t skip1 = 0; skip1 < seven.size(); ++skip1) {
        for (std::size_t skip2 = skip1 + 1; skip2 < seven.size(); ++skip2) {
            Cards five;
            for (std::size_t i = 0; i < seven.size(); ++i)
                if (i != skip1 && i != skip2) five.push_back(seven[i]);
            HandKey key = evaluateFive(five);
            if (best.cards.empty() || best.key < key) best = {key, five};
        }
    }
    std::sort(best.cards.begin(), best.cards.end(),
              [](const HCard& a, const HCard& b) { return b < a; });  // high to low
    return best;
}

std::string show(const Cards& cards) {
    std::ostringstream out;
    for (std::size_t i = 0; i < cards.size(); ++i) {
        if (i) out << ' ';
        out << std::left << std::setw(3) << toString(cards[i]);
    }
    return out.str();
}

}  // namespace

HoldEmGame::HoldEmGame(int numPlayers) : numPlayers_(numPlayers) {}

void HoldEmGame::play() {
    std::cout << "\n=== Texas Hold'em (no betting) with " << numPlayers_ << " players ===\n";
    dealer_ = randomInt(0, numPlayers_ - 1);
    do {
        playHand();
    } while (promptYesNo("\nDeal another hand?"));
}

void HoldEmGame::playHand() {
    HoldEmDeck deck;
    deck.shuffle();
    const int n = numPlayers_;

    std::cout << "\n--- Player " << dealer_ + 1 << " has the dealer button ---\n";

    // Two rounds of hole cards, starting with the player left of the dealer.
    std::vector<Cards> hole(n);
    for (int round = 0; round < 2; ++round)
        for (int i = 1; i <= n; ++i) hole[(dealer_ + i) % n].push_back(deck.draw());

    std::cout << "\nHole cards:\n";
    for (int p = 0; p < n; ++p)
        std::cout << "  Player " << std::setw(2) << p + 1 << ": " << show(hole[p]) << '\n';

    Cards board;
    auto dealStreet = [&](const std::string& street, int count) {
        waitForEnter("\nPress Enter to deal the " + street + "...");
        deck.draw();  // burn a card
        for (int i = 0; i < count; ++i) board.push_back(deck.draw());
        std::cout << "Board after the " << street << ": " << show(board) << '\n';
    };
    dealStreet("flop", 3);
    dealStreet("turn", 1);
    dealStreet("river", 1);

    // Showdown.
    std::cout << "\nShowdown:\n";
    std::vector<BestHand> results;
    for (int p = 0; p < n; ++p) {
        Cards seven = hole[p];
        seven.insert(seven.end(), board.begin(), board.end());
        results.push_back(bestOfSeven(seven));
        std::cout << "  Player " << std::setw(2) << p + 1 << ": " << show(hole[p])
                  << " | best five: " << show(results.back().cards) << " | "
                  << describe(results.back().key) << '\n';
    }

    HandKey top = results[0].key;
    for (const BestHand& r : results) top = std::max(top, r.key);
    std::vector<int> winners;
    for (int p = 0; p < n; ++p)
        if (results[p].key == top) winners.push_back(p + 1);

    if (winners.size() == 1) {
        std::cout << "\n*** Player " << winners[0] << " wins with: " << describe(top) << " ***\n";
    } else {
        std::cout << "\n*** Split pot between Players ";
        for (std::size_t i = 0; i < winners.size(); ++i)
            std::cout << (i ? (i + 1 == winners.size() ? " and " : ", ") : "") << winners[i];
        std::cout << ", each with: " << describe(top) << " ***\n";
    }

    dealer_ = (dealer_ + 1) % n;  // the button moves left
}
