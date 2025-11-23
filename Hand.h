#pragma once
#include <vector>
#include "Card.h"

class Hand {
public:
    std::vector<Card> cards;

    void replaceCard(int idx, const Card& c);
    int evaluateHand() const;  // ∑µªÿ ÷≈∆∆¿∑÷
};
