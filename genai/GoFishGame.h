#pragma once
// Go Fish played with any of the decks in Deck.h. You play against 1-4
// computer players. On your turn you ask another player for a rank you hold;
// if they have any cards of that rank they must hand them all over and you go
// again. Otherwise you "go fish" (draw from the pond) and, if you happen to
// draw the rank you asked for, you go again. Four cards of the same rank make
// a book. When every card is in a book, the player with the most books wins.

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "Deck.h"
#include "Game.h"
#include "Util.h"

template <typename D>
class GoFishGame : public Game {
public:
    using CardT = typename D::CardType;
    using R = typename D::RankType;

    explicit GoFishGame(int numComputerPlayers);
    void play() override;

private:
    struct Player {
        std::string name;
        bool human = false;
        std::vector<CardT> hand;
        std::vector<R> books;
        std::set<R> knownRanks;  // ranks everyone knows this player holds (they asked for them)
    };

    void reset();
    void deal();
    void runGame();
    bool gameOver() const;
    void takeTurn(int p);
    void chooseHuman(int p, const std::vector<int>& targets, int& target, R& rank);
    void chooseComputer(int p, const std::vector<int>& targets, int& target, R& rank);
    void fish(Player& pl);  // draw one card from the pond
    void layDownBooks(Player& pl);
    void showTable(int viewer) const;
    void announceWinner() const;

    static std::string rankLabel(R r) { return "[" + toString(r) + "]"; }
    static std::string verb(const Player& pl, const char* you, const char* them) {
        return pl.human ? you : them;
    }
    static std::string handText(std::vector<CardT> hand);
    static std::string booksText(const Player& pl);

    D deck_;
    std::vector<Player> players_;
};

// ---------------------------------------------------------------------------

template <typename D>
GoFishGame<D>::GoFishGame(int numComputerPlayers) {
    static const char* const names[] = {"Ada", "Grace", "Alan", "Linus"};
    Player you;
    you.name = "You";
    you.human = true;
    players_.push_back(you);
    for (int i = 0; i < numComputerPlayers && i < 4; ++i) {
        Player cpu;
        cpu.name = names[i];
        players_.push_back(cpu);
    }
}

template <typename D>
void GoFishGame<D>::play() {
    do {
        reset();
        std::cout << "\n=== Go Fish with a " << D::name << " ===\n"
                  << "Collect books of four cards of the same rank. Most books wins.\n";
        deal();
        runGame();
    } while (promptYesNo("\nPlay another game of Go Fish with this deck?"));
}

template <typename D>
void GoFishGame<D>::reset() {
    deck_ = D();
    deck_.shuffle();
    for (Player& pl : players_) {
        pl.hand.clear();
        pl.books.clear();
        pl.knownRanks.clear();
    }
}

template <typename D>
void GoFishGame<D>::deal() {
    const int handSize = players_.size() <= 3 ? 7 : 5;
    for (int i = 0; i < handSize; ++i)
        for (Player& pl : players_) pl.hand.push_back(deck_.draw());
    std::cout << "Each player is dealt " << handSize << " cards.\n";
    for (Player& pl : players_) layDownBooks(pl);  // a lucky deal can contain a book
}

template <typename D>
bool GoFishGame<D>::gameOver() const {
    if (!deck_.empty()) return false;
    for (const Player& pl : players_)
        if (!pl.hand.empty()) return false;
    return true;
}

template <typename D>
void GoFishGame<D>::runGame() {
    int current = 0;  // you go first
    while (!gameOver()) {
        takeTurn(current);
        current = (current + 1) % static_cast<int>(players_.size());
    }
    announceWinner();
}

template <typename D>
void GoFishGame<D>::takeTurn(int p) {
    Player& me = players_[p];
    if (me.hand.empty() && deck_.empty()) return;  // this player is out

    std::cout << "\n--- " << (me.human ? std::string("Your") : me.name + "'s") << " turn ---\n";

    bool again = true;
    while (again && !gameOver()) {
        again = false;

        if (me.hand.empty()) {
            if (deck_.empty()) return;
            std::cout << me.name << ' ' << verb(me, "have", "has") << " no cards, so "
                      << verb(me, "you draw", "draws") << " from the pond.\n";
            fish(me);
        }

        std::vector<int> targets;  // players who can be asked
        for (int i = 0; i < static_cast<int>(players_.size()); ++i)
            if (i != p && !players_[i].hand.empty()) targets.push_back(i);
        if (targets.empty()) {
            std::cout << "Nobody else has any cards to ask for.\n";
            if (!deck_.empty()) {
                fish(me);
                layDownBooks(me);
            }
            return;
        }

        int target = targets[0];
        R rank = me.hand[0].rank;
        if (me.human) chooseHuman(p, targets, target, rank);
        else chooseComputer(p, targets, target, rank);

        Player& them = players_[target];
        std::cout << me.name << ' ' << verb(me, "ask", "asks") << ' '
                  << (them.human ? "you" : them.name) << " for any " << rankLabel(rank)
                  << " cards.\n";
        me.knownRanks.insert(rank);

        // Hand over every card of the requested rank.
        std::vector<CardT> given;
        auto split = std::stable_partition(them.hand.begin(), them.hand.end(),
                                           [&](const CardT& c) { return !(c.rank == rank); });
        given.assign(split, them.hand.end());
        them.hand.erase(split, them.hand.end());
        them.knownRanks.erase(rank);

        if (!given.empty()) {
            std::cout << them.name << ' ' << verb(them, "hand", "hands") << " over " << given.size()
                      << " card" << (given.size() > 1 ? "s" : "") << ": " << handText(given) << '\n';
            me.hand.insert(me.hand.end(), given.begin(), given.end());
            layDownBooks(me);
            again = true;
        } else {
            std::cout << (them.human ? "You say" : them.name + " says") << ": \"Go Fish!\"\n";
            if (deck_.empty()) {
                std::cout << "The pond is empty.\n";
            } else {
                fish(me);
                if (me.hand.back().rank == rank) {
                    std::cout << "Lucky catch! " << (me.human ? "You" : me.name) << " fished up the "
                              << toString(me.hand.back()) << " and " << verb(me, "go", "goes")
                              << " again.\n";
                    again = true;
                }
                layDownBooks(me);
            }
        }
    }
}

template <typename D>
void GoFishGame<D>::fish(Player& pl) {
    pl.hand.push_back(deck_.draw());
    if (pl.human) std::cout << "You draw the " << pl.hand.back() << ".\n";
    else std::cout << pl.name << " draws a card.\n";
}

template <typename D>
void GoFishGame<D>::chooseHuman(int p, const std::vector<int>& targets, int& target, R& rank) {
    Player& me = players_[p];
    showTable(p);

    std::sort(me.hand.begin(), me.hand.end());
    std::vector<std::pair<R, int>> ranks;  // distinct ranks in hand, with counts
    for (const CardT& c : me.hand) {
        if (ranks.empty() || !(ranks.back().first == c.rank)) ranks.emplace_back(c.rank, 0);
        ++ranks.back().second;
    }

    std::cout << "Which rank will you ask for?\n";
    for (std::size_t i = 0; i < ranks.size(); ++i)
        std::cout << "  " << i + 1 << ") " << rankLabel(ranks[i].first) << " (you have "
                  << ranks[i].second << ")\n";
    rank = ranks[promptInt("Rank", 1, static_cast<int>(ranks.size())) - 1].first;

    if (targets.size() == 1) {
        target = targets[0];
        return;
    }
    std::cout << "Who will you ask?\n";
    for (std::size_t i = 0; i < targets.size(); ++i) {
        const Player& t = players_[targets[i]];
        std::cout << "  " << i + 1 << ") " << t.name << " (" << t.hand.size() << " cards)\n";
    }
    target = targets[promptInt("Player", 1, static_cast<int>(targets.size())) - 1];
}

template <typename D>
void GoFishGame<D>::chooseComputer(int p, const std::vector<int>& targets, int& target, R& rank) {
    const Player& me = players_[p];
    auto holds = [&](R r) {
        return std::any_of(me.hand.begin(), me.hand.end(), [&](const CardT& c) { return c.rank == r; });
    };

    // First choice: ask someone who is known to hold a rank we also hold.
    std::vector<std::pair<int, R>> leads;
    for (int t : targets)
        for (R r : players_[t].knownRanks)
            if (holds(r)) leads.emplace_back(t, r);

    if (!leads.empty()) {
        const auto& lead = leads[randomInt(0, static_cast<int>(leads.size()) - 1)];
        target = lead.first;
        rank = lead.second;
    } else {
        // Otherwise pick a random card from the hand (favors ranks we hold more of).
        rank = me.hand[randomInt(0, static_cast<int>(me.hand.size()) - 1)].rank;
        target = targets[randomInt(0, static_cast<int>(targets.size()) - 1)];
    }
}

template <typename D>
void GoFishGame<D>::layDownBooks(Player& pl) {
    std::map<R, int> counts;
    for (const CardT& c : pl.hand) ++counts[c.rank];
    for (auto [r, n] : counts) {
        while (n >= 4) {
            int removed = 0;
            for (auto it = pl.hand.begin(); it != pl.hand.end() && removed < 4;) {
                if (it->rank == r) {
                    it = pl.hand.erase(it);
                    ++removed;
                } else {
                    ++it;
                }
            }
            n -= 4;
            pl.books.push_back(r);
            std::cout << "*** " << pl.name << ' ' << verb(pl, "lay", "lays") << " down a book of "
                      << rankLabel(r) << "! ***\n";
        }
        if (n == 0) pl.knownRanks.erase(r);
    }
}

template <typename D>
std::string GoFishGame<D>::handText(std::vector<CardT> hand) {
    std::sort(hand.begin(), hand.end());
    std::string out;
    for (std::size_t i = 0; i < hand.size(); ++i) {
        if (i) out += (hand[i].rank == hand[i - 1].rank) ? ", " : " | ";
        out += toString(hand[i]);
    }
    return out;
}

template <typename D>
std::string GoFishGame<D>::booksText(const Player& pl) {
    if (pl.books.empty()) return "none";
    std::string out;
    for (R r : pl.books) out += rankLabel(r) + " ";
    return out;
}

template <typename D>
void GoFishGame<D>::showTable(int viewer) const {
    std::cout << "\nCards left in the pond: " << deck_.size() << '\n';
    for (int i = 0; i < static_cast<int>(players_.size()); ++i) {
        if (i == viewer) continue;
        const Player& pl = players_[i];
        std::cout << "  " << pl.name << ": " << pl.hand.size() << " cards, books: " << booksText(pl)
                  << '\n';
    }
    const Player& me = players_[viewer];
    std::cout << "  Your books: " << booksText(me) << '\n'
              << "  Your hand:  " << handText(me.hand) << "\n\n";
}

template <typename D>
void GoFishGame<D>::announceWinner() const {
    std::cout << "\n=== Game over: every card is in a book ===\n";
    std::size_t most = 0;
    for (const Player& pl : players_) {
        std::cout << "  " << pl.name << ": " << pl.books.size() << " book"
                  << (pl.books.size() == 1 ? "" : "s") << "  " << booksText(pl) << '\n';
        most = std::max(most, pl.books.size());
    }
    std::vector<std::string> winners;
    for (const Player& pl : players_)
        if (pl.books.size() == most) winners.push_back(pl.name);

    if (winners.size() == 1) {
        std::cout << (winners[0] == "You" ? "*** You win! ***\n" : "*** " + winners[0] + " wins! ***\n");
    } else {
        std::cout << "*** It's a tie between ";
        for (std::size_t i = 0; i < winners.size(); ++i)
            std::cout << (i ? (i + 1 == winners.size() ? " and " : ", ") : "") << winners[i];
        std::cout << " with " << most << " books each. ***\n";
    }
}
