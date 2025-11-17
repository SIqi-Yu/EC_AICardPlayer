// src/Deck.cpp
#include "Deck.h"
#include <algorithm>
#include <chrono>

Deck::Deck() {
    // Initialize a standard 52-card deck.
    cards.clear();
    for (int s = 0; s < 4; ++s) {
        for (int r = 2; r <= 14; ++r) {
            cards.push_back(Card{r, s});
        }
    }

    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned long>(seed));
}

void Deck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::draw() {
    Card c = cards.back();
    cards.pop_back();
    return c;
}

bool Deck::empty() const {
    return cards.empty();
}
