#pragma once
#include <vector>
#include "Hand.h"

enum Phase {
    TITLE,
    PLAYER_TURN,
    AI_TURN,
    RESULT
};

struct GameState {
    Hand playerHand;
    Hand aiHand;

    int playerScore = 0;
    int aiScore = 0;

    int playerScoreTemp = 0;
    int aiScoreTemp = 0;

    Phase currentPhase = TITLE;

    bool needRedraw = true;
    bool running = true;

    std::vector<int> selectedIndices;
};
