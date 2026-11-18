#pragma once

// Common interface for every game in the program.
class Game {
public:
    virtual ~Game() = default;
    // Plays one or more rounds, asking the user whether to keep going.
    virtual void play() = 0;
};
