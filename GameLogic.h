#pragma once
#include "GameState.h"
#include "Deck.h"
#include "AIPlayer.h"

class GameLogic {
public:
    GameLogic();

    void dealCards(GameState& state);
    void replaceCards(Hand& h, const std::vector<int>& indices, Deck& deck);
    int evaluate(const Hand& h) const;
    int evaluateWithCommunity(const Hand& h, const std::vector<Card>& community, int revealedCount) const;
    void updateScore(GameState& state, int pScore, int aScore);

private:
    Deck deck;
};
