#pragma once
#include <vector>
#include "Card.h"

class Deck {
public:
    Deck();
    void shuffle();
    Card draw();
    bool empty() const;

private:
    std::vector<Card> cards;
    int index;
};
