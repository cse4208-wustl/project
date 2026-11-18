#pragma once
// Random numbers and terminal input helpers.

#include <random>
#include <stdexcept>
#include <string>

// Shared random number generator (seeded from std::random_device unless seedRng is called).
std::mt19937& rng();
void seedRng(unsigned seed);
int randomInt(int lo, int hi);  // inclusive range

// Thrown when standard input is closed (Ctrl-D / end of a piped file).
struct EndOfInput : std::runtime_error {
    EndOfInput() : std::runtime_error("end of input") {}
};

// Keeps asking until the user types a whole number in [lo, hi].
int promptInt(const std::string& prompt, int lo, int hi);
// Accepts y/yes/n/no (any case).
bool promptYesNo(const std::string& prompt);
void waitForEnter(const std::string& prompt = "Press Enter to continue...");
