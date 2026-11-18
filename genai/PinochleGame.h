#pragma once
// Simple four-handed Pinochle: deal the 48-card deck and score each hand's melds.
// There is no bidding or trick play. Trump is the suit of the dealer's last card,
// which is turned face up.

#include "Game.h"

class PinochleGame : public Game {
public:
    void play() override;

private:
    void playHand();

    int dealer_ = 0;
};
