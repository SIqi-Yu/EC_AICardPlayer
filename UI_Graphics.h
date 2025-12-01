#pragma once
#include <string>
#include <vector>
#include "GameState.h"
#include "GameLogic.h"
#include "AIPlayer.h"
#include "fssimplewindow.h"
#include "yssimplesound.h"

class UI_Graphics {
public:
    UI_Graphics();
    ~UI_Graphics();
    void mainLoop();

private:
    GameState state;
    GameLogic logic;
    AIPlayer ai;
    YsSoundPlayer soundPlayer;
    YsSoundPlayer::SoundData backgroundMusic;
    bool backgroundLoaded = false;
    bool prevLeftButtonDown = false;
    bool showInstructions = false;

    // rendering
    void draw();
    void drawTitle();
    void drawPlayerHand();
    void drawAIHand();
    void drawButtons();
    void drawText(int x, int y, const std::string& text);
    void drawInstructionsOverlay();

    // input
    void processInput();
    bool inside(int mx, int my, int x, int y, int w, int h);

    int cardAt(int mx, int my);

    bool clickStart(int mx, int my);
    bool clickQuit(int mx, int my);
    bool clickInstructions(int mx, int my);
    bool clickInstructionsBack(int mx, int my);
    bool clickDone(int mx, int my);
    bool clickStand(int mx, int my);
    bool clickAgain(int mx, int my);
    bool clickTitle(int mx, int my);
};
