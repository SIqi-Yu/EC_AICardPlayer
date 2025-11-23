#pragma once
#include "GameState.h"
#include "GameLogic.h"
#include "AIPlayer.h"
#include <string>

class UI {
public:
    UI();
    void mainLoop();

private:
    GameState state;
    GameLogic logic;
    AIPlayer ai;

    void renderGame();
    void renderResult();
    void handlePlayerTurn();
};
