#pragma once
// Texas Hold'em without betting: deal hole cards, the flop, turn and river,
// then show everyone's best five-card hand and the winner(s).

#include "Game.h"

class HoldEmGame : public Game {
public:
    explicit HoldEmGame(int numPlayers);
    void play() override;

private:
    void playHand();

    int numPlayers_;
    int dealer_ = 0;  // index of the player holding the dealer button
};
