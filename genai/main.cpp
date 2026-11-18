// Terminal card games: Texas Hold'em (no betting), Pinochle (deal and meld),
// and Go Fish played with a Texas Hold'em, Pinochle or Uno deck.
//
// Usage: ./cardgames [--seed N]

#include <iostream>
#include <memory>
#include <string>

#include "Deck.h"
#include "GoFishGame.h"
#include "HoldEmGame.h"
#include "PinochleGame.h"
#include "Util.h"

namespace {

std::unique_ptr<Game> chooseGoFish() {
    std::cout << "\nWhich deck should Go Fish use?\n"
              << "  1) " << HoldEmDeck::name << '\n'
              << "  2) " << PinochleDeck::name << '\n'
              << "  3) " << UnoDeck::name << '\n';
    int deck = promptInt("Deck", 1, 3);
    int opponents = promptInt("How many computer opponents?", 1, 4);
    switch (deck) {
        case 1: return std::make_unique<GoFishGame<HoldEmDeck>>(opponents);
        case 2: return std::make_unique<GoFishGame<PinochleDeck>>(opponents);
        default: return std::make_unique<GoFishGame<UnoDeck>>(opponents);
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--seed" && i + 1 < argc) {
            seedRng(static_cast<unsigned>(std::stoul(argv[++i])));
        } else {
            std::cerr << "Usage: " << argv[0] << " [--seed N]\n";
            return 1;
        }
    }

    try {
        while (true) {
            std::cout << "\n========== Card Games ==========\n"
                      << "  1) Texas Hold'em (no betting)\n"
                      << "  2) Pinochle (deal and melds)\n"
                      << "  3) Go Fish\n"
                      << "  4) Quit\n";
            int choice = promptInt("Choose a game", 1, 4);

            std::unique_ptr<Game> game;
            switch (choice) {
                case 1:
                    game = std::make_unique<HoldEmGame>(promptInt("How many players?", 2, 10));
                    break;
                case 2:
                    game = std::make_unique<PinochleGame>();
                    break;
                case 3:
                    game = chooseGoFish();
                    break;
                default:
                    std::cout << "Goodbye!\n";
                    return 0;
            }
            game->play();
        }
    } catch (const EndOfInput&) {
        std::cout << "\nGoodbye!\n";
    }
    return 0;
}
