// include/GameLogic.h
#pragma once

#include <vector>
#include "Deck.h"
#include "Hand.h"
#include "GameState.h"

// Core game logic: dealing, replacement, scoring.
class GameLogic {
public:
    GameLogic();

    // Deal 7 cards to both player and AI, reset deck and shuffle.
    void dealCards(GameState& state);

    // Replace selected cards in the given hand with draws from the deck.
    void replaceCards(GameState& state, Hand& hand, const std::vector<int>& indices);

    // Evaluate hand strength (delegates to Hand::evaluateHand).
    int evaluateHand(const Hand& hand);

    // Update total score based on round result.
    void updateScore(GameState& state, int playerScoreThisRound, int aiScoreThisRound);

private:
    Deck deck;
};
