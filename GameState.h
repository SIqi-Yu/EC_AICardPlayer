#pragma once
#include <vector>
#include "Hand.h"

enum Phase {
    TITLE,
    PLAYER_TURN,
    AI_TURN,
    REVEAL,
    RESULT
};

struct GameState {
    Hand playerHand;
    Hand aiHand;
    std::vector<Card> communityCards;

    int playerScore = 0;
    int aiScore = 0;

    int playerChips = 1000;
    int aiChips = 1000;
    int roundBet = 0;
    bool playerDoubled = false;
    bool aiDoubled = false;

    int playerScoreTemp = 0;
    int aiScoreTemp = 0;

    Phase currentPhase = TITLE;

    bool needRedraw = true;
    bool running = true;

    std::vector<int> selectedIndices;
    int revealedCommunityCards = 0;
};
