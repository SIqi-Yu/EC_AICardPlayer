// src/main.cpp
#include "GameState.h"
#include "GameLogic.h"
#include "AIPlayer.h"
#include "UI.h"

int main() {
    GameState state;
    GameLogic logic;
    AIPlayer ai("Normal");
    UI ui(logic, ai);

    ui.mainLoop(state);
    return 0;
}
