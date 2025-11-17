// include/UI.h
#pragma once

#include <vector>
#include <string>

#include "GameState.h"
#include "GameLogic.h"
#include "AIPlayer.h"

// UI & interaction layer (responsibility: Siqi Yu)
class UI {
public:
    // The UI needs access to GameLogic and AIPlayer to trigger game actions.
    UI(GameLogic& logic, AIPlayer& ai);

    // Main event loop, called from main.cpp.
    void mainLoop(GameState& state);

private:
    GameLogic& gameLogic;
    AIPlayer& aiPlayer;

    int winW;
    int winH;

    // Selection flags for the player's cards.
    std::vector<bool> playerCardSelected;

    // ===== Rendering =====
    void renderTitle(const GameState& state);
    void renderGameScreen(const GameState& state);
    void renderResult(const GameState& state);

    // Draw a hand of cards starting at baseX, baseY.
    // If "selected" is provided, highlight selected cards.
    void drawHand(const Hand& hand,
                  int baseX,
                  int baseY,
                  bool isPlayer,
                  const std::vector<bool>* selected = nullptr);

    // Draw a simple rectangular button with a label.
    void drawButton(const std::string& label,
                    int x, int y, int w, int h,
                    bool hovered = false);

    // Draw text using bitmap font.
    void drawText(int x, int y, const std::string& text);

    // Helper for rank/suit display.
    std::string rankToString(int rank) const;
    std::string suitToString(int suit) const;

    // ===== Input handling =====
    void handleInput(GameState& state, bool& quit);
    void handleTitleInput(GameState& state, int mx, int my, int lb);
    void handlePlayerTurnInput(GameState& state, int mx, int my, int lb);
    void handleResultInput(GameState& state, int mx, int my, int lb);

    // ===== Utility helpers =====
    bool pointInRect(int x, int y, int rx, int ry, int rw, int rh) const;

    // Collect indices of currently selected player cards.
    std::vector<int> getSelectedCardIndices() const;
};
