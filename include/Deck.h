// include/Deck.h
#pragma once
#include <vector>
#include <random>
#include "Card.h"

class Deck {
public:
    Deck();

    // Shuffle the deck using a random engine.
    void shuffle();

    // Draw the top card (no safety check here; caller should ensure not empty).
    Card draw();

    // Check if the deck is empty.
    bool empty() const;

private:
    std::vector<Card> cards;
    std::mt19937 rng;
};
