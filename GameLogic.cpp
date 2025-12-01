#include "GameLogic.h"

GameLogic::GameLogic()
{
}

void GameLogic::dealCards(GameState& state)
{
    Deck localDeck;
    localDeck.shuffle();

    state.playerHand.cards.clear();
    state.aiHand.cards.clear();
    state.selectedIndices.clear();

    for (int i = 0; i < 7; ++i)
    {
        state.playerHand.cards.push_back(localDeck.draw());
        state.aiHand.cards.push_back(localDeck.draw());
    }

    state.currentPhase = PLAYER_TURN;
    state.needRedraw = true;
}

void GameLogic::replaceCards(Hand& h, const std::vector<int>& indices, Deck& deck)
{
    for (int idx : indices)
    {
        if (!deck.empty() && 0 <= idx && idx < static_cast<int>(h.cards.size()))
        {
            h.replaceCard(idx, deck.draw());
        }
    }
}

int GameLogic::evaluate(const Hand& h) const
{
    return h.evaluateHand();
}

void GameLogic::updateScore(GameState& state, int pScore, int aScore)
{
    state.playerScoreTemp = pScore;
    state.aiScoreTemp = aScore;

    if (pScore > aScore)
    {
        state.playerScore++;
    }
    else if (aScore > pScore)
    {
        state.aiScore++;
    }
}
