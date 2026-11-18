#include "Util.h"

#include <cctype>
#include <iostream>

std::mt19937& rng() {
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}

void seedRng(unsigned seed) { rng().seed(seed); }

int randomInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng());
}

namespace {
std::string readLine() {
    std::string line;
    if (!std::getline(std::cin, line)) throw EndOfInput();
    return line;
}

std::string trim(const std::string& s) {
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}
}  // namespace

int promptInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt << " [" << lo << "-" << hi << "]: " << std::flush;
        std::string line = trim(readLine());
        try {
            std::size_t used = 0;
            int value = std::stoi(line, &used);
            if (used == line.size() && value >= lo && value <= hi) return value;
        } catch (const std::exception&) {
            // fall through to the error message
        }
        std::cout << "Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

bool promptYesNo(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (y/n): " << std::flush;
        std::string line = trim(readLine());
        for (char& ch : line) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (line == "y" || line == "yes") return true;
        if (line == "n" || line == "no") return false;
        std::cout << "Please answer y or n.\n";
    }
}

void waitForEnter(const std::string& prompt) {
    std::cout << prompt << std::flush;
    readLine();
}
