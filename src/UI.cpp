// src/UI.cpp
#include "UI.h"

#include "fssimplewindow.h"
#include "ysglfontdata.h"

#include <cstdio>

// Simple layout parameters for cards
static const int CARD_WIDTH  = 80;
static const int CARD_HEIGHT = 120;
static const int CARD_GAP    = 20;

// Simple button struct for layout
struct Button {
    int x, y, w, h;
    std::string label;
};

UI::UI(GameLogic& logic, AIPlayer& ai)
    : gameLogic(logic),
      aiPlayer(ai),
      winW(1024),
      winH(768),
      playerCardSelected(7, false) {}

// Check if a point (x,y) is inside a rectangle (rx,ry,rw,rh).
bool UI::pointInRect(int x, int y, int rx, int ry, int rw, int rh) const {
    return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

// Convert numeric rank to string for display.
std::string UI::rankToString(int rank) const {
    if (rank >= 2 && rank <= 10) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "%d", rank);
        return std::string(buf);
    }
    switch (rank) {
    case 11: return "J";
    case 12: return "Q";
    case 13: return "K";
    case 14: return "A";
    default: return "?";
    }
}

// Convert suit id to a symbolic string.
std::string UI::suitToString(int suit) const {
    switch (suit) {
    case 0: return "♣";
    case 1: return "♦";
    case 2: return "♥";
    case 3: return "♠";
    default: return "?";
    }
}

// Draw text using the provided bitmap font.
void UI::drawText(int x, int y, const std::string& text) {
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap16x20(text.c_str());
}

// Draw a rectangular button with an optional hover effect.
void UI::drawButton(const std::string& label,
                    int x, int y, int w, int h,
                    bool hovered) {
    glColor3ub(hovered ? 200 : 150,
               hovered ? 200 : 150,
               hovered ? 50  : 150);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glColor3ub(0, 0, 0);
    drawText(x + 10, y + h - 10, label);
}

// Draw all cards in a hand; if "selected" is provided, highlight/offset them.
void UI::drawHand(const Hand& hand,
                  int baseX,
                  int baseY,
                  bool isPlayer,
                  const std::vector<bool>* selected) {
    for (int i = 0; i < (int)hand.cards.size(); ++i) {
        int x = baseX + i * (CARD_WIDTH + CARD_GAP);
        int y = baseY;

        bool isSel = false;
        if (selected && i < (int)selected->size()) {
            isSel = (*selected)[i];
        }

        // Visually lift selected cards for the player.
        if (isPlayer && isSel) {
            y -= 10;
        }

        // Card background
        glColor3ub(255, 255, isPlayer ? 200 : 230);
        if (isSel) {
            glColor3ub(255, 255, 150);
        }

        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + CARD_WIDTH, y);
        glVertex2i(x + CARD_WIDTH, y + CARD_HEIGHT);
        glVertex2i(x, y + CARD_HEIGHT);
        glEnd();

        // Card border
        glColor3ub(0, 0, 0);
        glBegin(GL_LINE_LOOP);
        glVertex2i(x, y);
        glVertex2i(x + CARD_WIDTH, y);
        glVertex2i(x + CARD_WIDTH, y + CARD_HEIGHT);
        glVertex2i(x, y + CARD_HEIGHT);
        glEnd();

        // Rank + suit text
        if (i < (int)hand.cards.size()) {
            const Card& c = hand.cards[i];
            std::string r = rankToString(c.rank);
            std::string s = suitToString(c.suit);
            glColor3ub(0, 0, 0);
            drawText(x + 5, y + 20, r);
            drawText(x + 5, y + 40, s);
        }
    }
}

// Collect indices of all selected player cards.
std::vector<int> UI::getSelectedCardIndices() const {
    std::vector<int> idx;
    for (int i = 0; i < (int)playerCardSelected.size(); ++i) {
        if (playerCardSelected[i]) {
            idx.push_back(i);
        }
    }
    return idx;
}

// ================== Rendering each phase ==================

// Render the title screen with project name and a "Start" button.
void UI::renderTitle(const GameState& /*state*/) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3ub(0, 0, 0);
    drawText(winW / 2 - 250, winH / 2 - 100,
             "Seven-Card Strategy Duel: Human vs AI Poker Showdown");
    drawText(winW / 2 - 120, winH / 2 - 60,
             "Click START to begin or ESC to quit");

    Button startBtn{ winW / 2 - 60, winH / 2, 120, 40, "Start" };

    int mx, my, lb, mb, rb;
    FsGetMouseState(lb, mb, rb, mx, my);
    bool hover = pointInRect(mx, my, startBtn.x, startBtn.y, startBtn.w, startBtn.h);

    drawButton(startBtn.label, startBtn.x, startBtn.y, startBtn.w, startBtn.h, hover);
}

// Render the main gameplay screen: both hands + Replace button.
void UI::renderGameScreen(const GameState& state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Header and scores
    glColor3ub(0, 0, 0);
    drawText(20, 30, "Seven-Card Strategy Duel");
    drawText(20, 60, "Player Score: " + std::to_string(state.playerScore));
    drawText(20, 90, "AI Score: " + std::to_string(state.aiScore));

    // AI hand (displayed but not interactive)
    drawText(50, 150, "AI Hand:");
    drawHand(state.aiHand, 50, 160, false, nullptr);

    // Player hand (clickable for selection)
    drawText(50, 380, "Player Hand (click cards to select for replacement):");

    // Make sure the selection array matches the current hand size.
    if ((int)playerCardSelected.size() < (int)state.playerHand.cards.size()) {
        playerCardSelected.resize(state.playerHand.cards.size(), false);
    }
    drawHand(state.playerHand, 50, 390, true, &playerCardSelected);

    // Replace button in the lower-right area
    Button replaceBtn{ winW - 200, winH - 100, 150, 40, "Replace" };

    int mx, my, lb, mb, rb;
    FsGetMouseState(lb, mb, rb, mx, my);
    bool hover = pointInRect(mx, my, replaceBtn.x, replaceBtn.y, replaceBtn.w, replaceBtn.h);

    drawButton(replaceBtn.label, replaceBtn.x, replaceBtn.y, replaceBtn.w, replaceBtn.h, hover);
}

// Render the result screen: final hands and updated scoreboard.
void UI::renderResult(const GameState& state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3ub(0, 0, 0);
    drawText(20, 40, "Round Result");
    drawText(20, 80, "Player Score: " + std::to_string(state.playerScore));
    drawText(20, 110, "AI Score: " + std::to_string(state.aiScore));

    // Show both final hands side by side.
    drawText(50, 160, "AI Final Hand:");
    drawHand(state.aiHand, 50, 170, false, nullptr);

    drawText(50, 380, "Player Final Hand:");
    drawHand(state.playerHand, 50, 390, true, nullptr);

    // "Play Again" and "Back to Title" buttons in the bottom-right.
    Button againBtn{ winW - 220, winH - 140, 180, 40, "Play Again" };
    Button backBtn { winW - 220, winH - 80,  180, 40, "Back to Title" };

    int mx, my, lb, mb, rb;
    FsGetMouseState(lb, mb, rb, mx, my);
    bool hoverAgain = pointInRect(mx, my, againBtn.x, againBtn.y, againBtn.w, againBtn.h);
    bool hoverBack  = pointInRect(mx, my, backBtn.x,  backBtn.y,  backBtn.w,  backBtn.h);

    drawButton(againBtn.label, againBtn.x, againBtn.y, againBtn.w, againBtn.h, hoverAgain);
    drawButton(backBtn.label,  backBtn.x,  backBtn.y,  backBtn.w,  backBtn.h,  hoverBack);
}

// ================== Input handling for each phase ==================

// Handle mouse clicks on the title screen (Start button).
void UI::handleTitleInput(GameState& state, int mx, int my, int lb) {
    Button startBtn{ winW / 2 - 60, winH / 2, 120, 40, "Start" };
    if (lb == 1 && pointInRect(mx, my, startBtn.x, startBtn.y, startBtn.w, startBtn.h)) {
        // Start a new round: deal cards and move to PLAYER_TURN.
        gameLogic.dealCards(state);
        playerCardSelected.assign(state.playerHand.cards.size(), false);
        state.currentPhase = PLAYER_TURN;
    }
}

// Handle clicks on cards and on the Replace button during PLAYER_TURN.
void UI::handlePlayerTurnInput(GameState& state, int mx, int my, int lb) {
    // Card selection: toggle on left click.
    if (lb == 1) {
        for (int i = 0; i < (int)state.playerHand.cards.size(); ++i) {
            int x = 50 + i * (CARD_WIDTH + CARD_GAP);
            int y = 390;  // same base Y used in renderGameScreen for the player hand
            int topY = y - 10; // allow a small offset for lifted selected cards
            if (pointInRect(mx, my, x, topY, CARD_WIDTH, CARD_HEIGHT + 10)) {
                if (i < (int)playerCardSelected.size()) {
                    playerCardSelected[i] = !playerCardSelected[i];
                }
            }
        }
    }

    // Replace button: trigger card replacement and transition to AI_TURN.
    Button replaceBtn{ winW - 200, winH - 100, 150, 40, "Replace" };
    if (lb == 1 && pointInRect(mx, my, replaceBtn.x, replaceBtn.y, replaceBtn.w, replaceBtn.h)) {
        auto idx = getSelectedCardIndices();
        if (!idx.empty()) {
            gameLogic.replaceCards(state, state.playerHand, idx);
        }
        state.currentPhase = AI_TURN;
    }
}

// Handle the Result screen buttons: Play Again / Back to Title.
void UI::handleResultInput(GameState& state, int mx, int my, int lb) {
    Button againBtn{ winW - 220, winH - 140, 180, 40, "Play Again" };
    Button backBtn { winW - 220, winH - 80,  180, 40, "Back to Title" };

    if (lb == 1) {
        if (pointInRect(mx, my, againBtn.x, againBtn.y, againBtn.w, againBtn.h)) {
            // Start a new round with fresh cards.
            gameLogic.dealCards(state);
            playerCardSelected.assign(state.playerHand.cards.size(), false);
            state.currentPhase = PLAYER_TURN;
        } else if (pointInRect(mx, my, backBtn.x, backBtn.y, backBtn.w, backBtn.h)) {
            // Return to title screen.
            state.currentPhase = TITLE;
        }
    }
}

// Dispatch input handling based on the current game phase.
void UI::handleInput(GameState& state, bool& quit) {
    int key = FsInkey();
    if (key == FSKEY_ESC) {
        quit = true;
        return;
    }

    int mx, my, lb, mb, rb;
    FsGetMouseState(lb, mb, rb, mx, my);

    switch (state.currentPhase) {
    case TITLE:
        handleTitleInput(state, mx, my, lb);
        break;
    case PLAYER_TURN:
        handlePlayerTurnInput(state, mx, my, lb);
        break;
    case RESULT:
        handleResultInput(state, mx, my, lb);
        break;
    default:
        break;
    }
}

// ================== Main loop ==================

// High-level UI event loop managing rendering and phase transitions.
void UI::mainLoop(GameState& state) {
    FsOpenWindow(16, 16, winW, winH, 1, "Seven-Card Strategy Duel");

    bool quit = false;

    while (!quit) {
        FsPollDevice();
        handleInput(state, quit);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, winW, winH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, winW - 1, winH - 1, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        switch (state.currentPhase) {
        case TITLE:
            renderTitle(state);
            break;
        case DEAL:
            // DEAL can be treated as an intermediate phase:
            // for simplicity, immediately go to PLAYER_TURN.
            state.currentPhase = PLAYER_TURN;
            break;
        case PLAYER_TURN:
            renderGameScreen(state);
            break;
        case AI_TURN:
        {
            // Render current state, show a short "thinking" message,
            // then let the AI replace cards and compute scores.
            renderGameScreen(state);
            glColor3ub(0, 0, 200);
            drawText(winW / 2 - 60, 120, "AI: Thinking...");
            FsSwapBuffers();
            FsSleep(400);  // small delay for visual feedback

            auto aiIdx = aiPlayer.decideCardsToReplace(state.aiHand);
            if (!aiIdx.empty()) {
                gameLogic.replaceCards(state, state.aiHand, aiIdx);
            }

            int pScore = gameLogic.evaluateHand(state.playerHand);
            int aScore = gameLogic.evaluateHand(state.aiHand);
            gameLogic.updateScore(state, pScore, aScore);

            state.currentPhase = RESULT;
            continue; // skip to next frame to render RESULT
        }
        case RESULT:
            renderResult(state);
            break;
        default:
            break;
        }

        FsSwapBuffers();
        FsSleep(20);
    }
}
