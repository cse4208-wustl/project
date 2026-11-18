#include "PinochleGame.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Deck.h"
#include "Util.h"

namespace {

using PCard = Card<PinochleRank, Suit>;
using Hand = std::vector<PCard>;

struct Meld {
    std::string name;
    int points;
};

// Standard single-deck meld values. A card may count in melds of different
// classes (e.g. a K of trump in a run and in kings around) but only once
// within the same class.
std::vector<Meld> findMelds(const Hand& hand, Suit trump) {
    int count[6][4] = {};  // [rank][suit]
    for (const PCard& c : hand) ++count[static_cast<int>(c.rank)][static_cast<int>(c.suit)];
    auto n = [&](PinochleRank r, Suit s) { return count[static_cast<int>(r)][static_cast<int>(s)]; };
    auto times = [](int k) { return k > 1 ? " x" + std::to_string(k) : std::string(); };

    std::vector<Meld> melds;
    const std::string trumpName = suitName(trump);

    // Class I: runs, marriages and the dix.
    int runs = std::min({n(PinochleRank::ace, trump), n(PinochleRank::ten, trump),
                         n(PinochleRank::king, trump), n(PinochleRank::queen, trump),
                         n(PinochleRank::jack, trump)});
    if (runs == 2) melds.push_back({"Double run in " + trumpName + " (A 10 K Q J x2)", 150});
    else if (runs == 1) melds.push_back({"Run in " + trumpName + " (A 10 K Q J)", 15});

    int royal = std::min(n(PinochleRank::king, trump), n(PinochleRank::queen, trump)) - runs;
    if (royal > 0) melds.push_back({"Royal marriage in " + trumpName + times(royal), 4 * royal});

    for (Suit s : allSuits) {
        if (s == trump) continue;
        int m = std::min(n(PinochleRank::king, s), n(PinochleRank::queen, s));
        if (m > 0) melds.push_back({"Marriage in " + suitName(s) + times(m), 2 * m});
    }

    int dix = n(PinochleRank::nine, trump);
    if (dix > 0) melds.push_back({"Dix (9 of trump)" + times(dix), dix});

    // Class II: one of a rank in every suit ("around").
    struct Around { PinochleRank rank; const char* name; int single; int dbl; };
    const Around arounds[] = {{PinochleRank::ace, "Aces", 10, 100},
                              {PinochleRank::king, "Kings", 8, 80},
                              {PinochleRank::queen, "Queens", 6, 60},
                              {PinochleRank::jack, "Jacks", 4, 40}};
    for (const Around& a : arounds) {
        int k = 2;
        for (Suit s : allSuits) k = std::min(k, n(a.rank, s));
        if (k == 2) melds.push_back({std::string("Double ") + a.name + " around", a.dbl});
        else if (k == 1) melds.push_back({std::string(a.name) + " around", a.single});
    }

    // Class III: pinochle (J of diamonds + Q of spades).
    int p = std::min(n(PinochleRank::jack, Suit::diamonds), n(PinochleRank::queen, Suit::spades));
    if (p == 2) melds.push_back({"Double pinochle (JD QS x2)", 30});
    else if (p == 1) melds.push_back({"Pinochle (JD QS)", 4});

    return melds;
}

// Prints a hand grouped by suit, highest rank first.
void printHand(Hand hand) {
    std::sort(hand.begin(), hand.end(), [](const PCard& a, const PCard& b) { return b < a; });
    for (Suit s : {Suit::spades, Suit::hearts, Suit::clubs, Suit::diamonds}) {
        std::cout << "    " << std::left << std::setw(9) << suitName(s) + ":";
        bool any = false;
        for (const PCard& c : hand) {
            if (c.suit != s) continue;
            std::cout << ' ' << c.rank;
            any = true;
        }
        if (!any) std::cout << " (void)";
        std::cout << '\n';
    }
}

}  // namespace

void PinochleGame::play() {
    std::cout << "\n=== Pinochle: deal and meld (4 players, partners 1 & 3 vs 2 & 4) ===\n";
    dealer_ = randomInt(0, 3);
    do {
        playHand();
    } while (promptYesNo("\nDeal another Pinochle hand?"));
}

void PinochleGame::playHand() {
    PinochleDeck deck;
    deck.shuffle();

    // Deal in packets of three, starting left of the dealer. The dealer gets the last card.
    std::vector<Hand> hands(4);
    int p = (dealer_ + 1) % 4;
    while (!deck.empty()) {
        for (int i = 0; i < 3; ++i) hands[p].push_back(deck.draw());
        p = (p + 1) % 4;
    }

    const PCard upCard = hands[dealer_].back();
    const Suit trump = upCard.suit;
    std::cout << "\nPlayer " << dealer_ + 1 << " deals. The dealer's last card (" << upCard
              << ") is turned up: " << suitName(trump) << " are trump.\n";

    int teamMeld[2] = {0, 0};
    for (int i = 0; i < 4; ++i) {
        std::cout << "\nPlayer " << i + 1 << " (Team " << (i % 2 == 0 ? "A" : "B") << ")\n";
        printHand(hands[i]);
        std::vector<Meld> melds = findMelds(hands[i], trump);
        int total = 0;
        if (melds.empty()) std::cout << "  Melds: none\n";
        else std::cout << "  Melds:\n";
        for (const Meld& m : melds) {
            std::cout << "    " << std::left << std::setw(40) << m.name << std::right << std::setw(4)
                      << m.points << '\n';
            total += m.points;
        }
        std::cout << "  Total meld: " << total << '\n';
        teamMeld[i % 2] += total;
    }

    std::cout << "\nTeam A (Players 1 & 3) meld: " << teamMeld[0]
              << "\nTeam B (Players 2 & 4) meld: " << teamMeld[1] << '\n';
    if (teamMeld[0] == teamMeld[1]) std::cout << "*** The teams melded the same. ***\n";
    else std::cout << "*** Team " << (teamMeld[0] > teamMeld[1] ? "A" : "B") << " has the bigger meld. ***\n";

    dealer_ = (dealer_ + 1) % 4;
}
