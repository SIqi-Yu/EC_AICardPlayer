// include/GameState.h
#pragma once

#include "Hand.h"

// Global game phases.
enum Phase { TITLE, DEAL, PLAYER_TURN, AI_TURN, RESULT };

// Global game state shared between UI and logic.
struct GameState {
    Hand playerHand;
    Hand aiHand;
    int playerScore = 0;
    int aiScore = 0;
    Phase currentPhase = TITLE;
};
