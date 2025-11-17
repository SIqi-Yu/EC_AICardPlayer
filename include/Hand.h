// include/Hand.h
#pragma once
#include <vector>
#include "Card.h"

// Represents a 7-card hand during the game.
struct Hand {
    std::vector<Card> cards; // expected size = 7 during play

    // Very simple evaluation stub: encodes a rough strength score.
    // You can replace this with full poker logic later.
    int evaluateHand() const;

    // Replace the card at index "idx" with a new card.
    void replaceCard(int idx, const Card& newCard);
};
