#pragma once
#include <chrono>
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
    YsSoundPlayer::SoundData failSound;
    YsSoundPlayer::SoundData successSound;
    bool backgroundLoaded = false;
    bool failSoundLoaded = false;
    bool successSoundLoaded = false;
    bool prevLeftButtonDown = false;
    bool showInstructions = false;
    bool showAdvice = false;
    std::chrono::steady_clock::time_point lastInteractionTime;
    bool waitingForDoubleChoice = false;
    std::string betStatusMessage;
    bool showEndPopup = false;
    std::string endPopupMessage;
    unsigned int titleBgTexture = 0;
    int titleBgWidth = 0;
    int titleBgHeight = 0;
    unsigned int introTexture = 0;
    int introTexWidth = 0;
    int introTexHeight = 0;
    unsigned int emblemTexture = 0;
    int emblemWidth = 0;
    int emblemHeight = 0;
    bool introActive = true;
    std::chrono::steady_clock::time_point introStartTime;

    // rendering
    void draw();
    void drawTitle();
    void drawIntroSplash();
    void drawPlayerHand();
    void drawAIHand();
    void drawCommunityCards();
    void drawRoundInfo();
    void drawButtons();
    void drawText(int x, int y, const std::string& text);
    void drawBackgroundCanvas();
    void drawInstructionsOverlay();
    void drawEndPopup();
    void drawAdvicePanel(const std::vector<std::string>& lines, float highlightIntensity);
    std::vector<std::string> buildAdviceStrings(const Hand& hand) const;
    void settleRound();
    bool loadTextureFromFile(const std::string& path,
                             unsigned int& tex,
                             int& outW,
                             int& outH,
                             bool flipVertically = true);

    // input
    void processInput();
    bool inside(int mx, int my, int x, int y, int w, int h) const;

    int cardAt(int mx, int my);

    bool clickStart(int mx, int my);
    bool clickQuit(int mx, int my);
    bool clickInstructions(int mx, int my);
    bool clickInstructionsBack(int mx, int my);
    bool clickAdviceButton(int mx, int my) const;
    bool clickDouble(int mx, int my) const;
    bool clickPass(int mx, int my) const;
    bool clickRevealNext(int mx, int my) const;
    bool clickDone(int mx, int my);
    bool clickStand(int mx, int my);
    bool clickAgain(int mx, int my);
    bool clickTitle(int mx, int my);
    bool clickEndPopupOk(int mx, int my) const;

    void revealNextCommunityCard();
    void handlePlayerBetChoice(bool doubleIt);
    void handleAIDoubleDecision();
    std::string applyDouble(bool byPlayer);
    void concludeRevealIfReady();
};
