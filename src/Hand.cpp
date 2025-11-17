// src/Hand.cpp
#include "Hand.h"

// Very naive evaluation: sum of ranks + small suit contribution.
// This gives a comparable integer, but is NOT real poker ranking.
int Hand::evaluateHand() const {
    int score = 0;
    for (const auto& c : cards) {
        score += c.rank * 10 + c.suit;
    }
    return score;
}

void Hand::replaceCard(int idx, const Card& newCard) {
    if (idx >= 0 && idx < (int)cards.size()) {
        cards[idx] = newCard;
    }
}
