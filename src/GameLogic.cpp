// src/GameLogic.cpp
#include "GameLogic.h"

GameLogic::GameLogic()
    : deck() {
}

// Initialize a new deck, shuffle, and deal 7 cards to each player.
void GameLogic::dealCards(GameState& state) {
    deck = Deck();
    deck.shuffle();

    state.playerHand.cards.clear();
    state.aiHand.cards.clear();

    for (int i = 0; i < 7; ++i) {
        if (!deck.empty()) {
            state.playerHand.cards.push_back(deck.draw());
        }
        if (!deck.empty()) {
            state.aiHand.cards.push_back(deck.draw());
        }
    }
}

// Replace selected cards at the given indices with newly drawn cards.
void GameLogic::replaceCards(GameState& /*state*/, Hand& hand, const std::vector<int>& indices) {
    for (int idx : indices) {
        if (!deck.empty()) {
            hand.replaceCard(idx, deck.draw());
        }
    }
}

// Delegate to Hand::evaluateHand for actual scoring.
int GameLogic::evaluateHand(const Hand& hand) {
    return hand.evaluateHand();
}

// Increment win counters based on this round's scores.
void GameLogic::updateScore(GameState& state, int pScore, int aScore) {
    if (pScore > aScore) {
        state.playerScore++;
    } else if (aScore > pScore) {
        state.aiScore++;
    }
    // Tie: no score change.
}
