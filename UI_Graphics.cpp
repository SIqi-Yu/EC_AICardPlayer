#include "UI_Graphics.h"
#include "ysglfontdata.h"
#include "Deck.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <vector>
#include <array>
#include <cstring>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const int WINDOW_W = 1200;
static const int WINDOW_H = 700;

static const int CARD_W = 90;
static const int CARD_H = 130;
static const int CARD_GAP = 20;
static const int CARD_X_OFFSET = 90;

static inline int PlayerBaseX()
{
    const int totalW = 7 * CARD_W + 6 * CARD_GAP;
    return (WINDOW_W - totalW) / 2 + CARD_X_OFFSET;
}

static inline int PlayerBaseY()
{
    return 210;
}

static inline int AIBaseX()
{
    return PlayerBaseX();
}

static inline int AIBaseY()
{
    return 450;
}

static inline int CommunityBaseX()
{
    const int totalW = 3 * CARD_W + 2 * CARD_GAP;
    return (WINDOW_W - totalW) / 2 + CARD_X_OFFSET;
}

static inline int CommunityBaseY()
{
    return 300;
}

static const int BUTTON_WIDTH = 220;
static const int BUTTON_HEIGHT = 32;
static const int BUTTON_GAP = 4;
static const int BASE_BET = 50;

static inline int ButtonsBaseX()
{
    return WINDOW_W - BUTTON_WIDTH - 30;
}

static inline int ButtonRowY(int row)
{
    const int totalHeight = 3 * BUTTON_HEIGHT + 2 * BUTTON_GAP;
    int y = WINDOW_H - totalHeight - 30;
    int minY = AIBaseY() + CARD_H + 10;
    return std::max(y, minY) + row * (BUTTON_HEIGHT + BUTTON_GAP);
}

static constexpr int MAIN_FONT_W = 16;
static constexpr int MAIN_FONT_H = 20;
static constexpr int ADVICE_FONT_W = 10;
static constexpr int ADVICE_FONT_H = 14;
static constexpr int INSTR_FONT_W = 12;
static constexpr int INSTR_FONT_H = 16;

static int TextWidth(const std::string& s)
{
    return static_cast<int>(s.size()) * MAIN_FONT_W;
}

static int TextWidth(const char* s)
{
    return static_cast<int>(std::strlen(s)) * MAIN_FONT_W;
}

static int AdviceTextWidth(const std::string& s)
{
    return static_cast<int>(s.size()) * ADVICE_FONT_W;
}

static int InstrTextWidth(const std::string& s)
{
    return static_cast<int>(s.size()) * INSTR_FONT_W;
}

static void DrawSmallText(int x, int y, const std::string& s)
{
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap16x20(s.c_str());
}

static void DrawAdviceText(int x, int y, const std::string& s)
{
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap10x14(s.c_str());
}

static void DrawInstrText(int x, int y, const std::string& s)
{
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap12x16(s.c_str());
}

static std::string DescribeHandScore(int score);

static void DrawSuitSymbol(int cx, int cy, int suit)
{
    switch (suit)
    {
    case 0:
        glColor3ub(0, 0, 0);
        break;
    case 1:
        glColor3ub(220, 40, 40);
        break;
    case 2:
        glColor3ub(0, 0, 0);
        break;
    case 3:
        glColor3ub(220, 40, 40);
        break;
    }

    if (suit == 0)
    {
        int r = 12;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * M_PI;
            int x = cx + (int)(r * std::cos(ang));
            int y = cy - 5 - (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2i(cx - r, cy - 5);
        glVertex2i(cx + r, cy - 5);
        glVertex2i(cx, cy + r + 8);
        glEnd();
    }
    else if (suit == 1)
    {
        int r = 10;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx - r / 2, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx - r / 2 + (int)(r * std::cos(ang));
            int y = cy - 5 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx + r / 2, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx + r / 2 + (int)(r * std::cos(ang));
            int y = cy - 5 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2i(cx - r - 2, cy);
        glVertex2i(cx + r + 2, cy);
        glVertex2i(cx, cy + r + 10);
        glEnd();
    }
    else if (suit == 2)
    {
        int r = 8;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx, cy - r);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx + (int)(r * std::cos(ang));
            int y = cy - r + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx - r, cy + 1);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx - r + (int)(r * std::cos(ang));
            int y = cy + 1 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx + r, cy + 1);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx + r + (int)(r * std::cos(ang));
            int y = cy + 1 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex2i(cx - 4, cy + r + 3);
        glVertex2i(cx + 4, cy + r + 3);
        glVertex2i(cx, cy + r + 12);
        glEnd();
    }
    else if (suit == 3)
    {
        int r = 13;
        glBegin(GL_QUADS);
        glVertex2i(cx, cy - r);
        glVertex2i(cx + r, cy);
        glVertex2i(cx, cy + r);
        glVertex2i(cx - r, cy);
        glEnd();
    }
}

static std::string RankToString(int rank)
{
    switch (rank)
    {
    case 11: return "J";
    case 12: return "Q";
    case 13: return "K";
    case 14: return "A";
    default:
        char buf[8];
        std::snprintf(buf, sizeof(buf), "%d", rank);
        return buf;
    }
}

static void DrawCardAt(int x, int y, const Card& c, bool selected)
{
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(x + 4, y + 4);
    glVertex2i(x + 4 + CARD_W, y + 4);
    glVertex2i(x + 4 + CARD_W, y + 4 + CARD_H);
    glVertex2i(x + 4, y + 4 + CARD_H);
    glEnd();

    glColor3ub(245, 245, 245);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();

    glColor3ub(40, 40, 40);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();

    if (selected)
    {
        glColor3ub(220, 40, 40);
        glBegin(GL_LINE_LOOP);
        glVertex2i(x - 2, y - 2);
        glVertex2i(x + CARD_W + 2, y - 2);
        glVertex2i(x + CARD_W + 2, y + CARD_H + 2);
        glVertex2i(x - 2, y + CARD_H + 2);
        glEnd();
    }

    std::string rStr = RankToString(c.rank);
    glColor3ub(0, 0, 0);
    if (c.suit == 1 || c.suit == 3)
    {
        glColor3ub(200, 40, 40);
    }
    DrawSmallText(x + 8, y + 20, rStr);

    int cx = x + CARD_W / 2;
    int cy = y + CARD_H / 2;
    DrawSuitSymbol(cx, cy, c.suit);
}


UI_Graphics::UI_Graphics()
    : ai("Normal")
{
    state.currentPhase = TITLE;
    state.needRedraw = true;
    state.running = true;
    state.playerScore = 0;
    state.aiScore = 0;
    state.playerScoreTemp = 0;
    state.aiScoreTemp = 0;
    state.roundBet = 0;
    waitingForDoubleChoice = false;
    lastInteractionTime = std::chrono::steady_clock::now();
    betStatusMessage.clear();
    showEndPopup = false;
    endPopupMessage.clear();
    if (YSOK == backgroundMusic.LoadWav("las-vegas-407027.wav"))
    {
        backgroundLoaded = true;
    }
    else
    {
        printf("Failed to load background music.\n");
    }
    if (YSOK == failSound.LoadWav("fail-234710.wav"))
    {
        failSoundLoaded = true;
    }
    else
    {
        printf("Failed to load fail sound.\n");
    }
    if (YSOK == successSound.LoadWav("purchase-success.wav"))
    {
        successSoundLoaded = true;
    }
    else
    {
        printf("Failed to load success sound.\n");
    }
}

UI_Graphics::~UI_Graphics()
{
    if (backgroundLoaded)
    {
        soundPlayer.Stop(backgroundMusic);
    }
    soundPlayer.End();
    if (titleBgTexture != 0)
    {
        glDeleteTextures(1, &titleBgTexture);
        titleBgTexture = 0;
    }
    if (introTexture != 0)
    {
        glDeleteTextures(1, &introTexture);
        introTexture = 0;
    }
    if (emblemTexture != 0)
    {
        glDeleteTextures(1, &emblemTexture);
        emblemTexture = 0;
    }
}

bool UI_Graphics::loadTextureFromFile(const std::string& path,
                                      unsigned int& tex,
                                      int& outW,
                                      int& outH,
                                      bool flipVertically)
{
    // Allow caller to control flipping so we don't unintentionally mirror icons
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);
    int w = 0, h = 0, comp = 0;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &comp, 4);
    if (!data)
    {
        std::fprintf(stderr, "Failed to load title background image: %s\n", path.c_str());
        return false;
    }

    if (tex != 0)
    {
        glDeleteTextures(1, &tex);
        tex = 0;
    }

    GLuint newTex = 0;
    glGenTextures(1, &newTex);
    glBindTexture(GL_TEXTURE_2D, newTex);
    GLint prevUnpack = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpack);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpack);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    tex = newTex;
    outW = w;
    outH = h;
    return true;
}

void UI_Graphics::mainLoop()
{
    FsOpenWindow(100, 100, WINDOW_W, WINDOW_H, 1, "Seven-Card Strategy Duel");
    glClearColor(0, 0, 0, 0);
    if (titleBgTexture == 0)
    {
        loadTextureFromFile("AdobeStock_267112986.jpeg", titleBgTexture, titleBgWidth, titleBgHeight, false);
    }
    if (introTexture == 0)
    {
        loadTextureFromFile("AdobeStock_512097593.jpeg", introTexture, introTexWidth, introTexHeight, false);
    }
    if (emblemTexture == 0)
    {
        // Keep emblem upright; the source image already has correct orientation.
        loadTextureFromFile("AdobeStock_529389711.png", emblemTexture, emblemWidth, emblemHeight, false);
    }
    introStartTime = std::chrono::steady_clock::now();
    introActive = true;
    soundPlayer.Start();
    if (backgroundLoaded)
    {
        soundPlayer.PlayBackground(backgroundMusic);
    }

    while (state.running)
    {
        FsPollDevice();
        int key = FsInkey();
        if (key == FSKEY_ESC)
        {
            break;
        }

        processInput();

        if (state.needRedraw)
        {
            draw();
            if (introActive)
            {
                state.needRedraw = true;
            }
            else
            {
                state.needRedraw = false;
            }
        }

        FsSleep(10);
    }
}


void UI_Graphics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (introActive)
    {
        drawIntroSplash();
        if (introActive)
        {
            FsSwapBuffers();
            state.needRedraw = true;
            return;
        }
    }

    switch (state.currentPhase)
    {
    case TITLE:
        drawTitle();
        break;
    case PLAYER_TURN:
        drawPlayerHand();
        drawCommunityCards();
        drawRoundInfo();
        drawButtons();
        break;
    case AI_TURN:
        drawPlayerHand();
        drawButtons();
        break;
    case REVEAL:
        drawPlayerHand();
        drawCommunityCards();
        drawAIHand();
        drawRoundInfo();
        drawButtons();
        break;
    case RESULT:
        drawPlayerHand();
        drawCommunityCards();
        drawAIHand();
        drawRoundInfo();
        drawButtons();
        break;
    default:
        break;
    }

    FsSwapBuffers();
}

void UI_Graphics::drawText(int x, int y, const std::string& text)
{
    glColor3ub(255, 255, 255);
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap20x32(text.c_str());
}

void UI_Graphics::drawBackgroundCanvas()
{
    if (titleBgTexture != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, titleBgTexture);
        glColor3ub(255, 255, 255);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(WINDOW_W, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(WINDOW_W, WINDOW_H);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, WINDOW_H);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(0, 0, 0, 190);
        glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(WINDOW_W, 0);
        glVertex2i(WINDOW_W, WINDOW_H);
        glVertex2i(0, WINDOW_H);
        glEnd();
        glDisable(GL_BLEND);
    }
    else
    {
        glBegin(GL_QUADS);
        glColor3ub(8, 6, 20);
        glVertex2i(0, 0);
        glColor3ub(18, 10, 35);
        glVertex2i(WINDOW_W, 0);
        glColor3ub(4, 2, 12);
        glVertex2i(WINDOW_W, WINDOW_H);
        glColor3ub(2, 0, 6);
        glVertex2i(0, WINDOW_H);
        glEnd();
    }
}

void UI_Graphics::drawIntroSplash()
{
    drawBackgroundCanvas();

    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - introStartTime).count();
    const double duration = 3.2;
    double t = std::min(elapsed, duration);
    double progress = t / duration;
    double scale = 1.15 - 0.2 * progress; // gently zoom out

    double fadeStart = duration * 0.55;
    double alpha = 1.0;
    if (t > fadeStart)
    {
        alpha = std::max(0.0, 1.0 - (t - fadeStart) / (duration - fadeStart));
    }

    if (introTexture != 0)
    {
        double drawW = WINDOW_W * scale;
        double drawH = WINDOW_H * scale;
        double x0 = (WINDOW_W - drawW) / 2.0;
        double y0 = (WINDOW_H - drawH) / 2.0;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, introTexture);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(static_cast<float>(alpha), static_cast<float>(alpha),
            static_cast<float>(alpha), static_cast<float>(alpha));
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2d(x0, y0);
        glTexCoord2f(1.0f, 0.0f); glVertex2d(x0 + drawW, y0);
        glTexCoord2f(1.0f, 1.0f); glVertex2d(x0 + drawW, y0 + drawH);
        glTexCoord2f(0.0f, 1.0f); glVertex2d(x0, y0 + drawH);
        glEnd();
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    if (elapsed >= duration)
    {
        introActive = false;
        state.needRedraw = true;
    }
    else
    {
        state.needRedraw = true;
    }
}

void UI_Graphics::drawTitle()
{
    drawBackgroundCanvas();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0, 0, 0, 180);
    glBegin(GL_QUADS);
    glVertex2i(150, 110);
    glVertex2i(WINDOW_W - 150, 110);
    glVertex2i(WINDOW_W - 150, 220);
    glVertex2i(150, 220);
    glEnd();
    glDisable(GL_BLEND);

    glColor3ub(240, 240, 255);
    drawText(230, 150, "Seven-Card Strategy Duel");
    glColor3ub(250, 197, 67);
    DrawSmallText(360, 210, "Stack the odds · Double down · Rule the felt");
    if (emblemTexture != 0)
    {
        double targetH = 260.0; // enlarge emblem further
        double aspect = (emblemHeight > 0) ? static_cast<double>(emblemWidth) / emblemHeight : 1.0;
        double drawW = targetH * aspect;
        double drawH = targetH;
        double rightX = WINDOW_W + 70;
        double leftX = rightX - drawW;
        double y = 60;
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, emblemTexture);
        glColor4ub(255, 255, 255, 255);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2d(leftX, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2d(rightX, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2d(rightX, y + drawH);
        glTexCoord2f(0.0f, 1.0f); glVertex2d(leftX, y + drawH);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    auto drawTitleButton = [&](int y, const char* label, unsigned char r, unsigned char g, unsigned char b)
    {
        int bx = 430, bw = 220, bh = 62;
        glBegin(GL_QUADS);
        glColor3ub(r / 2, g / 2, b / 2);
        glVertex2i(bx, y);
        glVertex2i(bx + bw, y);
        glColor3ub(r, g, b);
        glVertex2i(bx + bw, y + bh);
        glVertex2i(bx, y + bh);
        glEnd();

        glColor3ub(255, 230, 190);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(bx, y);
        glVertex2i(bx + bw, y);
        glVertex2i(bx + bw, y + bh);
        glVertex2i(bx, y + bh);
        glEnd();

        glColor3ub(255, 255, 255);
        int textW = TextWidth(label);
        int textX = bx + (bw - textW) / 2;
        int textY = y + (bh + MAIN_FONT_H) / 2 - 2;
        DrawSmallText(textX, textY, label);
    };

    drawTitleButton(310, "START", 180, 50, 80);
    drawTitleButton(400, "HOW TO PLAY", 80, 120, 200);
    drawTitleButton(490, "QUIT", 120, 120, 120);

    if (showInstructions)
    {
        drawInstructionsOverlay();
    }
    if (showEndPopup)
    {
        drawEndPopup();
    }
}

static void DrawHiddenCardAt(int x, int y)
{
    glColor3ub(40, 40, 40);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();
    glColor3ub(200, 40, 40);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();
    glColor3ub(200, 200, 200);
    int textX = x + (CARD_W - MAIN_FONT_W) / 2;
    int textY = y + (CARD_H + MAIN_FONT_H) / 2 - 4;
    DrawSmallText(textX, textY, "?");
}

void UI_Graphics::drawPlayerHand()
{
    drawBackgroundCanvas();

    const int headerH = 165;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(6, 5, 16, 230);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_W, 0);
    glVertex2i(WINDOW_W, headerH);
    glVertex2i(0, headerH);
    glEnd();
    glDisable(GL_BLEND);
    glColor3ub(120, 80, 200);
    glBegin(GL_QUADS);
    glVertex2i(0, headerH - 3);
    glVertex2i(WINDOW_W, headerH - 3);
    glVertex2i(WINDOW_W, headerH);
    glVertex2i(0, headerH);
    glEnd();

    glColor3ub(240, 240, 255);
    drawText(60, 70, "Your Hand:");

    char scoreBuf[128];
    std::snprintf(scoreBuf, sizeof(scoreBuf),
        "Score  P %d : Bot %d",
        state.playerScore, state.aiScore);
    int scoreW = TextWidth(scoreBuf);
    int scoreX = std::max(40, WINDOW_W - scoreW - 30);
    glColor3ub(255, 210, 110);
    DrawSmallText(scoreX, 35, scoreBuf);
    char chipBuf[128];
    std::snprintf(chipBuf, sizeof(chipBuf),
        "Chips  P %d : Bot %d",
        state.playerChips, state.aiChips);
    int chipW = TextWidth(chipBuf);
    int chipX = std::max(40, WINDOW_W - chipW - 30);
    glColor3ub(180, 230, 255);
    DrawSmallText(chipX, 65, chipBuf);

    if (!state.communityCards.empty())
    {
        char sharedBuf[128];
        std::snprintf(sharedBuf, sizeof(sharedBuf),
            "Shared Cards: %d / %d cards revealed",
            std::min(state.revealedCommunityCards, (int)state.communityCards.size()),
            (int)state.communityCards.size());
        glColor3ub(240, 235, 150);
        DrawSmallText(60, headerH + 20, sharedBuf);
    }

    const char* adviceLabel = showAdvice ? "Advice: ON" : "Advice: OFF";
    int adviceTextW = TextWidth(adviceLabel);
    int adviceBtnX = 60;
    int adviceBtnY = WINDOW_H - 55;
    int adviceBtnW = std::max(150, adviceTextW + 24);
    int adviceBtnH = 34;
    glColor3ub(20, 10, 20);
    glBegin(GL_QUADS);
    glVertex2i(adviceBtnX, adviceBtnY);
    glVertex2i(adviceBtnX + adviceBtnW, adviceBtnY);
    glVertex2i(adviceBtnX + adviceBtnW, adviceBtnY + adviceBtnH);
    glVertex2i(adviceBtnX, adviceBtnY + adviceBtnH);
    glEnd();
    glColor3ub(255, 90, 90);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(adviceBtnX, adviceBtnY);
    glVertex2i(adviceBtnX + adviceBtnW, adviceBtnY);
    glVertex2i(adviceBtnX + adviceBtnW, adviceBtnY + adviceBtnH);
    glVertex2i(adviceBtnX, adviceBtnY + adviceBtnH);
    glEnd();
    glColor3ub(255, 255, 255);
    int adviceTextX = adviceBtnX + (adviceBtnW - adviceTextW) / 2;
    int adviceTextY = adviceBtnY + (adviceBtnH + MAIN_FONT_H) / 2 - 3;
    DrawSmallText(adviceTextX, adviceTextY, adviceLabel);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(10, 10, 16, 220);
    glBegin(GL_QUADS);
    glVertex2i(40, headerH + 25);
    glVertex2i(WINDOW_W - 40, headerH + 25);
    glVertex2i(WINDOW_W - 40, PlayerBaseY() + CARD_H + 30);
    glVertex2i(40, PlayerBaseY() + CARD_H + 30);
    glEnd();
    glDisable(GL_BLEND);

    auto now = std::chrono::steady_clock::now();
    double idleSeconds = std::chrono::duration<double>(now - lastInteractionTime).count();
    bool idleForHint = idleSeconds >= 10.0;
    bool adviceHint = showAdvice && idleForHint;
    float advicePulse = 0.0f;
    if (adviceHint)
    {
        double phase = std::fmod(idleSeconds - 10.0, 1.2);
        advicePulse = 0.4f + 0.6f * static_cast<float>(0.5 * (std::sin(phase * 2.0 * M_PI) + 1.0));
        glColor3ub(255, 120, 120);
        DrawSmallText(adviceBtnX, adviceBtnY - 20, "Advice flashing: click to refresh tips!");
    }
    else if (showAdvice)
    {
        glColor3ub(200, 220, 255);
        DrawSmallText(adviceBtnX, adviceBtnY - 20, "Advice panel active.");
    }
    else if (idleForHint)
    {
        glColor3ub(255, 190, 120);
        DrawSmallText(adviceBtnX, adviceBtnY - 22, "Idle? Advice ready with a click.");
        glColor3ub(255, 220, 220);
        glLineWidth(1.8f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(adviceBtnX - 3, adviceBtnY - 3);
        glVertex2i(adviceBtnX + adviceBtnW + 3, adviceBtnY - 3);
        glVertex2i(adviceBtnX + adviceBtnW + 3, adviceBtnY + adviceBtnH + 3);
        glVertex2i(adviceBtnX - 3, adviceBtnY + adviceBtnH + 3);
        glEnd();
    }
    if (advicePulse > 0.0f)
    {
        glLineWidth(1.5f + advicePulse * 3.0f);
        glColor3ub(255, 200, 180);
        glBegin(GL_LINE_LOOP);
        glVertex2i(adviceBtnX - 2, adviceBtnY - 2);
        glVertex2i(adviceBtnX + adviceBtnW + 2, adviceBtnY - 2);
        glVertex2i(adviceBtnX + adviceBtnW + 2, adviceBtnY + adviceBtnH + 2);
        glVertex2i(adviceBtnX - 2, adviceBtnY + adviceBtnH + 2);
        glEnd();
    }
    int baseX = PlayerBaseX();
    int baseY = PlayerBaseY();

    for (int i = 0; i < (int)state.playerHand.cards.size(); ++i)
    {
        int x = baseX + i * (CARD_W + CARD_GAP);
        int y = baseY;

        bool selected =
            std::find(state.selectedIndices.begin(),
                state.selectedIndices.end(),
                i) != state.selectedIndices.end();

        DrawCardAt(x, y, state.playerHand.cards[i], selected);
    }

    if (state.currentPhase == PLAYER_TURN)
    {
        int infoTop = 100;
        int infoBottom = 150;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(18, 15, 32, 220);
        glBegin(GL_QUADS);
        glVertex2i(40, infoTop);
        glVertex2i(WINDOW_W - 40, infoTop);
        glVertex2i(WINDOW_W - 40, infoBottom);
        glVertex2i(40, infoBottom);
        glEnd();
        glDisable(GL_BLEND);

        glColor3ub(255, 210, 150);
        char hintBuf[128];
        std::snprintf(hintBuf, sizeof(hintBuf),
            "Select cards to swap (currently %zu chosen, up to 7).",
            state.selectedIndices.size());
        DrawSmallText(60, infoTop + 18, hintBuf);
        glColor3ub(200, 220, 255);
        DrawSmallText(60, infoTop + 38, "Click cards to toggle selection, DONE to draw new ones, STAND to keep.");
    }

    if (showAdvice && state.currentPhase == PLAYER_TURN)
    {
        bool pulseState = false;
        if (adviceHint)
        {
            double phase = std::fmod(idleSeconds - 10.0, 1.0);
            pulseState = phase < 0.5;
        }
        drawAdvicePanel(buildAdviceStrings(state.playerHand), advicePulse);
    }
    else if (showAdvice && state.currentPhase != PLAYER_TURN)
    {
        // Auto-hide advice once the player leaves their turn to avoid overlaps in later phases.
        showAdvice = false;
    }
}

void UI_Graphics::drawCommunityCards()
{
    if (state.communityCards.empty())
    {
        return;
    }

    int baseX = CommunityBaseX();
    int y = CommunityBaseY();
    int total = static_cast<int>(state.communityCards.size());
    int revealCount = state.currentPhase == RESULT ? total : state.revealedCommunityCards;

    for (int i = 0; i < total; ++i)
    {
        int x = baseX + i * (CARD_W + CARD_GAP);
        if (i < revealCount)
        {
            DrawCardAt(x, y, state.communityCards[i], false);
        }
        else
        {
            DrawHiddenCardAt(x, y);
        }
    }
}

void UI_Graphics::drawRoundInfo()
{
    if (state.currentPhase != REVEAL && state.currentPhase != RESULT)
    {
        return;
    }

    const int panelX = 50;
    const int panelY = AIBaseY() + CARD_H + 20;
    const int panelW = ButtonsBaseX() - panelX - 20;
    const int panelH = (state.currentPhase == RESULT ? 130 : 80);
    glColor3ub(16, 12, 24);
    glBegin(GL_QUADS);
    glVertex2i(panelX, panelY);
    glVertex2i(panelX + panelW, panelY);
    glVertex2i(panelX + panelW, panelY + panelH);
    glVertex2i(panelX, panelY + panelH);
    glEnd();
    glColor3ub(220, 140, 70);
    glBegin(GL_LINE_LOOP);
    glVertex2i(panelX, panelY);
    glVertex2i(panelX + panelW, panelY);
    glVertex2i(panelX + panelW, panelY + panelH);
    glVertex2i(panelX, panelY + panelH);
    glEnd();

    int x = panelX + 20;
    int y = panelY + 30;

    if (state.currentPhase == RESULT)
    {
        std::string playerDesc = DescribeHandScore(state.playerScoreTemp);
        std::string aiDesc = DescribeHandScore(state.aiScoreTemp);
        const char* winnerLabel = "Draw";
        if (state.playerScoreTemp > state.aiScoreTemp)
        {
            winnerLabel = "Player";
        }
        else if (state.aiScoreTemp > state.playerScoreTemp)
        {
            winnerLabel = "Bot";
        }

    glColor3ub(255, 210, 140);
        DrawSmallText(x, y, ("Player: " + playerDesc).c_str());
        y += 26;
        glColor3ub(255, 140, 140);
        DrawSmallText(x, y, ("Bot: " + aiDesc).c_str());
        y += 26;
        glColor3ub(200, 255, 180);
        DrawSmallText(x, y, ("Winner: " + std::string(winnerLabel)).c_str());
        y += 26;

        if (!betStatusMessage.empty())
        {
            glColor3ub(255, 220, 150);
            DrawSmallText(x, y, betStatusMessage.c_str());
            y += 24;
        }

        glColor3ub(220, 220, 220);
        DrawSmallText(x, y, "Use AGAIN to re-deal or TITLE to exit.");
        return;
    }

    if (state.currentPhase == REVEAL)
    {
        glColor3ub(255, 220, 150);
        char bufStatus[160];
        std::snprintf(bufStatus, sizeof(bufStatus),
            "Bet: %d  Shared cards revealed %d / %d.",
            state.roundBet,
            state.revealedCommunityCards,
            (int)state.communityCards.size());
        DrawSmallText(x, y, bufStatus);
        y += 26;

        if (!betStatusMessage.empty())
        {
            glColor3ub(255, 210, 150);
            DrawSmallText(x, y, betStatusMessage.c_str());
            y += 24;
        }

        glColor3ub(255, 190, 120);
        if (waitingForDoubleChoice)
        {
            DrawSmallText(x, y, "Choose DOUBLE or PASS.");
        }
        else if (state.revealedCommunityCards < (int)state.communityCards.size())
        {
            DrawSmallText(x, y, "Click REVEAL for the next shared card.");
        }
        else
        {
            DrawSmallText(x, y, "All shared cards revealed. Continue to showdown.");
        }
    }
}

void UI_Graphics::drawAIHand()
{
    glColor3ub(255, 0, 0);
    const bool showFinal = (state.currentPhase == RESULT);
    const char* label = showFinal ? "Bot Final Hand:" : "Bot Hand (hidden):";
    DrawSmallText(80, AIBaseY() - 20, label);

    int baseX = AIBaseX();
    int baseY = AIBaseY();

    for (int i = 0; i < (int)state.aiHand.cards.size(); ++i)
    {
        int x = baseX + i * (CARD_W + CARD_GAP);
        if (showFinal)
        {
            DrawCardAt(x, baseY, state.aiHand.cards[i], false);
        }
        else
        {
            DrawHiddenCardAt(x, baseY);
        }
    }

    if (showFinal)
    {
        return;
    }
}

void UI_Graphics::drawButtons()
{
    int bx = ButtonsBaseX();

    auto drawButton = [&](int row, const char* label)
    {
        int by = ButtonRowY(row);
        glBegin(GL_QUADS);
        glColor3ub(25, 18, 30);
        glVertex2i(bx, by);
        glColor3ub(55, 40, 70);
        glVertex2i(bx + BUTTON_WIDTH, by);
        glColor3ub(40, 20, 45);
        glVertex2i(bx + BUTTON_WIDTH, by + BUTTON_HEIGHT);
        glColor3ub(15, 8, 20);
        glVertex2i(bx, by + BUTTON_HEIGHT);
        glEnd();

        glColor3ub(255, 200, 120);
        glBegin(GL_LINE_LOOP);
        glVertex2i(bx, by);
        glVertex2i(bx + BUTTON_WIDTH, by);
        glVertex2i(bx + BUTTON_WIDTH, by + BUTTON_HEIGHT);
        glVertex2i(bx, by + BUTTON_HEIGHT);
        glEnd();

        glColor3ub(255, 255, 255);
        int textW = TextWidth(label);
        int textX = bx + (BUTTON_WIDTH - textW) / 2;
        int textY = by + (BUTTON_HEIGHT + MAIN_FONT_H) / 2 - 2;
        DrawSmallText(textX, textY, label);
    };

    if (state.currentPhase == PLAYER_TURN)
    {
        drawButton(0, "DONE");
        drawButton(1, "STAND");
        drawButton(2, "TITLE");
    }
    else if (state.currentPhase == REVEAL)
    {
        if (waitingForDoubleChoice)
        {
            drawButton(0, "DOUBLE");
            drawButton(1, "PASS");
        }
        else
        {
            if (state.revealedCommunityCards < (int)state.communityCards.size())
            {
                drawButton(0, "REVEAL");
            }
            else
            {
                drawButton(0, "SHOWDOWN");
            }
        }
    }
    else if (state.currentPhase == RESULT)
    {
        drawButton(0, "AGAIN");
        drawButton(1, "TITLE");
        drawButton(2, "QUIT");
    }
}

static const char* HandCategoryName(int cat)
{
    switch (cat)
    {
    case 8: return "Straight Flush";
    case 7: return "Four of a Kind";
    case 6: return "Full House";
    case 5: return "Flush";
    case 4: return "Straight";
    case 3: return "Three of a Kind";
    case 2: return "Two Pair";
    case 1: return "One Pair";
    default: return "High Card";
    }
}

static std::string RankName(int rank)
{
    if (rank <= 1)
    {
        return "A";
    }
    if (rank == 11) return "J";
    if (rank == 12) return "Q";
    if (rank == 13) return "K";
    if (rank == 14) return "A";
    return std::to_string(rank);
}

static std::string DescribeHandScore(int score)
{
    int category = score / 1000000;
    int hi1 = (score / 10000) % 100;
    int hi2 = (score / 100) % 100;
    int hi3 = score % 100;

    std::ostringstream oss;
    oss << HandCategoryName(category);
    if (hi1 > 0)
    {
        oss << " (" << RankName(hi1);
        if (hi2 > 0 || hi3 > 0)
        {
            oss << "/" << RankName(hi2);
            if (hi3 > 0)
            {
                oss << "/" << RankName(hi3);
            }
        }
        oss << ")";
    }
    return oss.str();
}

void UI_Graphics::drawInstructionsOverlay()
{
    glColor3ub(15, 15, 15);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_W, 0);
    glVertex2i(WINDOW_W, WINDOW_H);
    glVertex2i(0, WINDOW_H);
    glEnd();

    glColor3ub(255, 255, 255);
    int titleW = InstrTextWidth("Game Instructions");
    DrawInstrText((WINDOW_W - titleW) / 2, 110, "Game Instructions");

    std::vector<std::string> lines = {
        "Both you and the bot draw 7 private cards. You may swap any of yours once.",
        "Click cards to select them, then press DONE to draw new ones.",
        "Use STAND if you want to keep the current hand and skip swapping.",
        "Three shared cards sit between you and the bot. Reveal them one by one.",
        "After every reveal, you may choose DOUBLE or PASS.",
        "The bot can also choose to DOUBLE based on its hand strength.",
        "The round bet doubles whenever either side chooses DOUBLE.",
        "Hands are compared using classic poker rankings (strongest to weakest):",
        "  Straight Flush > Four of a Kind > Full House > Flush > Straight",
        "  Three of a Kind > Two Pair > One Pair > High Card",
        "Winner takes the current bet from the opponent's chip stack."
    };
    glColor3ub(220, 220, 220);
    int textY = 170;
    int textX = 80;
    int maxWidth = WINDOW_W - 2 * textX;
    std::vector<std::string> wrapped;
    for (const auto& line : lines)
    {
        std::istringstream iss(line);
        std::string word, current;
        while (iss >> word)
        {
            std::string test = current.empty() ? word : current + " " + word;
            if (InstrTextWidth(test) > maxWidth)
            {
                if (!current.empty())
                {
                    wrapped.push_back(current);
                }
                current = word;
            }
            else
            {
                current = test;
            }
        }
        if (!current.empty())
        {
            wrapped.push_back(current);
        }
        wrapped.push_back(""); // extra line spacing between sentences
    }

    for (const auto& line : wrapped)
    {
        if (line.empty())
        {
            textY += INSTR_FONT_H / 2;
            continue;
        }
        int lineW = InstrTextWidth(line);
        int x = (WINDOW_W - lineW) / 2;
        DrawInstrText(x, textY, line);
        textY += INSTR_FONT_H + 6;
    }

    int bx = (WINDOW_W - BUTTON_WIDTH) / 2;
    int by = WINDOW_H - BUTTON_HEIGHT - 40;
    glColor3ub(60, 60, 60);
    glBegin(GL_QUADS);
    glVertex2i(bx, by);
    glVertex2i(bx + BUTTON_WIDTH, by);
    glVertex2i(bx + BUTTON_WIDTH, by + BUTTON_HEIGHT);
    glVertex2i(bx, by + BUTTON_HEIGHT);
    glEnd();
    glColor3ub(255, 255, 255);
    int backTextW = TextWidth("BACK");
    int backTextX = bx + (BUTTON_WIDTH - backTextW) / 2;
    int backTextY = by + (BUTTON_HEIGHT + MAIN_FONT_H) / 2 - 2;
    DrawSmallText(backTextX, backTextY, "BACK");
}

void UI_Graphics::drawEndPopup()
{
    const std::string footer = "Click OK to return to the title screen.";
    int contentW = std::max(TextWidth(endPopupMessage), TextWidth(footer));
    int w = contentW + 140;
    w = std::max(620, w);
    w = std::min(w, WINDOW_W - 60); // keep inside window
    const int h = 260;
    const int x = (WINDOW_W - w) / 2;
    const int y = (WINDOW_H - h) / 2;

    glColor4ub(0, 0, 0, 200);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_W, 0);
    glVertex2i(WINDOW_W, WINDOW_H);
    glVertex2i(0, WINDOW_H);
    glEnd();

    glColor3ub(40, 40, 40);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glColor3ub(255, 255, 255);
    DrawSmallText(x + 40, y + 40, "Match Result");

    glColor3ub(255, 210, 140);
    DrawSmallText(x + 40, y + 90, endPopupMessage);

    glColor3ub(200, 200, 200);
    DrawSmallText(x + 40, y + 130, footer);

    const int btnW = 100;
    const int btnH = 40;
    const int btnX = x + w / 2 - btnW / 2;
    const int btnY = y + h - btnH - 30;
    glColor3ub(90, 90, 90);
    glBegin(GL_QUADS);
    glVertex2i(btnX, btnY);
    glVertex2i(btnX + btnW, btnY);
    glVertex2i(btnX + btnW, btnY + btnH);
    glVertex2i(btnX, btnY + btnH);
    glEnd();
    glColor3ub(255, 255, 255);
    int okTextW = TextWidth("OK");
    int okTextX = btnX + (btnW - okTextW) / 2;
    int okTextY = btnY + (btnH + MAIN_FONT_H) / 2 - 2;
    DrawSmallText(okTextX, okTextY, "OK");
}

std::vector<std::string> UI_Graphics::buildAdviceStrings(const Hand& hand) const
{
    std::vector<std::string> lines;
    int rankCount[15] = {0};
    int suitCount[4] = {0};

    for (const auto& c : hand.cards)
    {
        if (2 <= c.rank && c.rank <= 14)
        {
            rankCount[c.rank]++;
        }
        if (0 <= c.suit && c.suit < 4)
        {
            suitCount[c.suit]++;
        }
    }

    int maxRankCount = 0;
    int maxRank = 2;
    for (int r = 2; r <= 14; ++r)
    {
        if (rankCount[r] > maxRankCount)
        {
            maxRankCount = rankCount[r];
            maxRank = r;
        }
    }

    std::string typeLabel = "Hand outlook: ";
    if (maxRankCount >= 4)
    {
        typeLabel += "Four-of-a-kind potential";
    }
    else if (maxRankCount == 3)
    {
        typeLabel += "Trips or full-house draw";
    }
    else if (maxRankCount == 2)
    {
        typeLabel += "Pair(s) to build on";
    }
    else
    {
        typeLabel += "High card, stay sharp on swaps";
    }
    lines.push_back(typeLabel);

    int bestSuit = 0;
    int bestSuitCount = 0;
    const char* suitNames[] = {"♠ Spades", "♥ Hearts", "♣ Clubs", "♦ Diamonds"};
    for (int s = 0; s < 4; ++s)
    {
        if (suitCount[s] > bestSuitCount)
        {
            bestSuitCount = suitCount[s];
            bestSuit = s;
        }
    }
    if (bestSuitCount >= 4)
    {
        std::string flushLine = "Flush draw on ";
        flushLine += suitNames[bestSuit];
        flushLine += " (" + std::to_string(bestSuitCount) + " cards).";
        lines.push_back(flushLine);
    }
    else
    {
        std::string suitLine = "Favor " + std::string(suitNames[bestSuit]);
        suitLine += " (" + std::to_string(bestSuitCount) + " cards)";
        lines.push_back(suitLine);
    }

    auto replaceIndices = ai.decideCardsToReplace(hand);
    if (!replaceIndices.empty())
    {
        std::ostringstream oss;
        oss << "Replace " << replaceIndices.size() << " card";
        if (replaceIndices.size() > 1)
        {
            oss << "s";
        }
        oss << ": ";
        for (size_t i = 0; i < replaceIndices.size(); ++i)
        {
            oss << (replaceIndices[i] + 1);
            if (i + 1 < replaceIndices.size())
            {
                oss << ", ";
            }
        }
        oss << " (positions).";
        lines.push_back(oss.str());
    }
    else
    {
        lines.push_back("Stand pat! This opening hand looks solid.");
    }

    return lines;
}

void UI_Graphics::drawAdvicePanel(const std::vector<std::string>& lines, float highlightIntensity)
{
    const int w = 560;
    const int h = 150 + static_cast<int>(lines.size()) * 24;
    const int x = 40;
    const int y = 420;

    glBegin(GL_QUADS);
    glColor3ub(6, 6, 32);
    glVertex2i(x, y);
    glColor3ub(16, 16, 70);
    glVertex2i(x + w, y);
    glColor3ub(8, 8, 44);
    glVertex2i(x + w, y + h);
    glColor3ub(4, 4, 20);
    glVertex2i(x, y + h);
    glEnd();

    float intensity = highlightIntensity;
    if (intensity < 0.0f)
    {
        intensity = 0.0f;
    }
    else if (intensity > 1.0f)
    {
        intensity = 1.0f;
    }
    if (intensity > 0.0f)
    {
        glLineWidth(3.0f + 3.0f * intensity);
        glColor4f(1.0f, 0.4f + 0.6f * intensity, 0.45f + 0.5f * intensity, 0.85f);
    }
    else
    {
        glLineWidth(2.0f);
        glColor3ub(205, 70, 90);
    }
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glColor3ub(255, 255, 255);
    DrawAdviceText(x + 22, y + 32, "Player Advice Panel");

    glColor3ub(220, 240, 255);
    int textY = y + 60;
    for (const auto& line : lines)
    {
        DrawAdviceText(x + 22, textY, line);
        textY += 24;
    }

    if (intensity > 0.0f)
    {
        glColor3ub(255, 180, 180);
        DrawAdviceText(x + 22, y + h - 16, "⚡ Flashing for extra guidance.");
    }
    else
    {
        glColor3ub(200, 200, 220);
        DrawAdviceText(x + 22, y + h - 16, "Idle to trigger a glow.");
    }
}

void UI_Graphics::settleRound()
{
    waitingForDoubleChoice = false;
    int payout = state.roundBet;
    if (state.playerScoreTemp == state.aiScoreTemp || payout <= 0)
    {
        betStatusMessage = "Round pushes. No chips exchanged.";
        state.roundBet = 0;
        state.needRedraw = true;
    }
    else
    {
        if (state.playerScoreTemp > state.aiScoreTemp)
        {
            int bet = std::min(payout, state.aiChips);
            state.aiChips -= bet;
            state.playerChips += bet;
            char buf[128];
            std::snprintf(buf, sizeof(buf), "Player wins %d chips!", bet);
            betStatusMessage = buf;
        }
        else
        {
            int bet = std::min(payout, state.playerChips);
            state.playerChips -= bet;
            state.aiChips += bet;
            char buf[128];
            std::snprintf(buf, sizeof(buf), "Bot wins %d chips.", bet);
            betStatusMessage = buf;
        }
        state.roundBet = 0;
        state.needRedraw = true;
    }

    if (state.playerChips <= 0 || state.aiChips <= 0)
    {
        showEndPopup = true;
        if (state.playerChips <= 0 && state.aiChips <= 0)
        {
            endPopupMessage = "Both players went broke!";
        }
        else if (state.playerChips <= 0)
        {
            endPopupMessage = "You are out of chips. Bot wins the match.";
            if (failSoundLoaded)
            {
                soundPlayer.PlayOneShot(failSound);
            }
        }
        else
        {
            endPopupMessage = "Bot is out of chips. You win the match!";
            if (successSoundLoaded)
            {
                soundPlayer.PlayOneShot(successSound);
            }
        }
        state.currentPhase = TITLE;
        waitingForDoubleChoice = false;
        state.selectedIndices.clear();
        state.revealedCommunityCards = 0;
        betStatusMessage.clear();
        showInstructions = false;
        state.needRedraw = true;
    }
}

bool UI_Graphics::clickInstructions(int mx, int my)
{
    return inside(mx, my, 430, 380, 200, 60);
}

bool UI_Graphics::clickInstructionsBack(int mx, int my)
{
    int bx = (WINDOW_W - BUTTON_WIDTH) / 2;
    int by = WINDOW_H - BUTTON_HEIGHT - 40;
    return inside(mx, my, bx, by, BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickAdviceButton(int mx, int my) const
{
    const int adviceBtnX = 60;
    const int adviceBtnY = WINDOW_H - 50;
    const int adviceBtnW = 140;
    const int adviceBtnH = 32;
    return inside(mx, my, adviceBtnX, adviceBtnY, adviceBtnW, adviceBtnH);
}

bool UI_Graphics::clickDouble(int mx, int my) const
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(0), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickPass(int mx, int my) const
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(1), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickRevealNext(int mx, int my) const
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(0), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickEndPopupOk(int mx, int my) const
{
    const int w = 500;
    const int h = 220;
    const int x = (WINDOW_W - w) / 2;
    const int y = (WINDOW_H - h) / 2;
    const int btnW = 100;
    const int btnH = 40;
    const int btnX = x + w / 2 - btnW / 2;
    const int btnY = y + h - btnH - 30;
    return inside(mx, my, btnX, btnY, btnW, btnH);
}

void UI_Graphics::revealNextCommunityCard()
{
    int total = static_cast<int>(state.communityCards.size());
    if (state.revealedCommunityCards >= total)
    {
        concludeRevealIfReady();
        return;
    }

    state.revealedCommunityCards++;
    waitingForDoubleChoice = true;

    char buf[128];
    std::snprintf(buf, sizeof(buf), "Shared card %d revealed.", state.revealedCommunityCards);
    betStatusMessage = buf;
    state.needRedraw = true;
}

void UI_Graphics::handlePlayerBetChoice(bool doubleIt)
{
    if (doubleIt)
    {
        betStatusMessage = applyDouble(true);
    }
    else
    {
        betStatusMessage = "Player passes.";
    }

    handleAIDoubleDecision();
    waitingForDoubleChoice = false;
    concludeRevealIfReady();
    state.needRedraw = true;
}

void UI_Graphics::handleAIDoubleDecision()
{
    if (state.playerChips <= 0 || state.aiChips <= 0)
    {
        return;
    }

    int maxBet = std::min(state.playerChips, state.aiChips);
    if (maxBet <= 0)
    {
        return;
    }

    int revealed = state.revealedCommunityCards;
    if (revealed <= 0)
    {
        return;
    }

    int aiScore = logic.evaluateWithCommunity(state.aiHand, state.communityCards, revealed);
    int playerScore = logic.evaluateWithCommunity(state.playerHand, state.communityCards, revealed);
    const int threshold = 50000; // lower threshold so bot reacts more often
    int margin = aiScore - playerScore;

    // If already at cap, just pass.
    if (state.roundBet >= maxBet)
    {
        if (betStatusMessage.empty())
        {
            betStatusMessage = "Bot passes.";
        }
        else
        {
            betStatusMessage += " Bot passes.";
        }
        return;
    }

    bool aiDoubles = false;
    if (margin > threshold)
    {
        aiDoubles = true;
    }
    else if (margin >= 0)
    {
        // Slight edge: sometimes push the bet.
        aiDoubles = (std::rand() % 100) < 30;
    }

    if (aiDoubles)
    {
        std::string aiMsg = applyDouble(false);
        if (!aiMsg.empty())
        {
            if (!betStatusMessage.empty())
            {
                betStatusMessage += " ";
            }
            betStatusMessage += aiMsg;
        }
    }
    else
    {
        if (betStatusMessage.empty())
        {
            betStatusMessage = "Bot passes.";
        }
        else
        {
            betStatusMessage += " Bot passes.";
        }
    }
}

std::string UI_Graphics::applyDouble(bool byPlayer)
{
    int maxBet = std::min(state.playerChips, state.aiChips);
    if (maxBet <= 0)
    {
        return "No chips left to raise.";
    }

    if (state.roundBet <= 0)
    {
        state.roundBet = std::min(BASE_BET, maxBet);
    }
    else
    {
        state.roundBet = std::min(maxBet, state.roundBet * 2);
    }

    char buf[128];
    std::snprintf(buf, sizeof(buf), "%s doubles! Bet is now %d.",
        byPlayer ? "Player" : "Bot", state.roundBet);
    return std::string(buf);
}

void UI_Graphics::concludeRevealIfReady()
{
    int total = static_cast<int>(state.communityCards.size());
    if (state.revealedCommunityCards < total || waitingForDoubleChoice)
    {
        return;
    }

    int pScore = logic.evaluateWithCommunity(state.playerHand, state.communityCards, total);
    int aScore = logic.evaluateWithCommunity(state.aiHand, state.communityCards, total);
    logic.updateScore(state, pScore, aScore);
    settleRound();
    if (showEndPopup)
    {
        return;
    }
    state.currentPhase = RESULT;
    state.needRedraw = true;
}

bool UI_Graphics::inside(int mx, int my, int x, int y, int w, int h) const
{
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

int UI_Graphics::cardAt(int mx, int my)
{
    int baseX = PlayerBaseX();
    int baseY = PlayerBaseY();

    for (int i = 0; i < (int)state.playerHand.cards.size(); ++i)
    {
        int x = baseX + i * (CARD_W + CARD_GAP);
        int y = baseY;
        if (inside(mx, my, x, y, CARD_W, CARD_H))
        {
            return i;
        }
    }
    return -1;
}

bool UI_Graphics::clickStart(int mx, int my)
{
    return inside(mx, my, 430, 300, 200, 60);
}

bool UI_Graphics::clickQuit(int mx, int my)
{
    if (state.currentPhase == TITLE)
    {
        return inside(mx, my, 430, 460, 200, 60);
    }
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(2), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickDone(int mx, int my)
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(0), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickStand(int mx, int my)
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(1), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickAgain(int mx, int my)
{
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(0), BUTTON_WIDTH, BUTTON_HEIGHT);
}

bool UI_Graphics::clickTitle(int mx, int my)
{
    if (state.currentPhase == PLAYER_TURN)
    {
        return inside(mx, my, ButtonsBaseX(), ButtonRowY(2), BUTTON_WIDTH, BUTTON_HEIGHT);
    }
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(1), BUTTON_WIDTH, BUTTON_HEIGHT);
}

void UI_Graphics::processInput()
{
    if (introActive)
    {
        return;
    }
    int lb, mb, rb, mx, my;
    FsGetMouseState(lb, mb, rb, mx, my);

    const bool leftDown = (lb != 0);
    if (!leftDown)
    {
        prevLeftButtonDown = false;
        return;
    }
    if (prevLeftButtonDown)
    {
        return;
    }
    prevLeftButtonDown = true;
    lastInteractionTime = std::chrono::steady_clock::now();

    switch (state.currentPhase)
    {
    case TITLE:
        if (showEndPopup)
        {
            if (clickEndPopupOk(mx, my))
            {
                showEndPopup = false;
                state.needRedraw = true;
            }
            break;
        }

        if (showInstructions)
        {
            if (clickInstructionsBack(mx, my))
            {
                showInstructions = false;
                state.needRedraw = true;
            }
            break;
        }

        if (clickStart(mx, my))
        {
            state.playerChips = 1000;
            state.aiChips = 1000;
            state.playerScore = 0;
            state.aiScore = 0;
            logic.dealCards(state);
            state.selectedIndices.clear();
            state.roundBet = 0;
            waitingForDoubleChoice = false;
            betStatusMessage.clear();
            state.currentPhase = PLAYER_TURN;
            state.needRedraw = true;
        }
        else if (clickInstructions(mx, my))
        {
            showInstructions = true;
            state.needRedraw = true;
        }
        else if (clickQuit(mx, my))
        {
            state.running = false;
        }
        break;

    case PLAYER_TURN:
    {
        if (clickAdviceButton(mx, my))
        {
            showAdvice = !showAdvice;
            state.needRedraw = true;
            break;
        }

        int idx = cardAt(mx, my);
        if (idx != -1)
        {
            auto it = std::find(state.selectedIndices.begin(),
                state.selectedIndices.end(), idx);
            if (it == state.selectedIndices.end())
            {
                state.selectedIndices.push_back(idx);
            }
            else
            {
                state.selectedIndices.erase(it);
            }
            state.needRedraw = true;
            break;
        }

        if (clickDone(mx, my))
        {
            Deck deck;
            deck.shuffle();

            logic.replaceCards(state.playerHand, state.selectedIndices, deck);
            state.selectedIndices.clear();

            auto aiIndices = ai.decideCardsToReplace(state.aiHand);
            logic.replaceCards(state.aiHand, aiIndices, deck);

            showAdvice = false;
            state.currentPhase = REVEAL;
            state.revealedCommunityCards = 0;
            state.playerScoreTemp = 0;
            state.aiScoreTemp = 0;
            int baseBet = std::min(BASE_BET, std::min(state.playerChips, state.aiChips));
            state.roundBet = baseBet;
            waitingForDoubleChoice = false;
            betStatusMessage = "Shared cards hidden. Click REVEAL to continue.";
            state.needRedraw = true;
        }
        else if (clickStand(mx, my))
        {
            Deck deck;
            deck.shuffle();

            auto aiIndices = ai.decideCardsToReplace(state.aiHand);
            logic.replaceCards(state.aiHand, aiIndices, deck);

            showAdvice = false;
            state.currentPhase = REVEAL;
            state.revealedCommunityCards = 0;
            state.playerScoreTemp = 0;
            state.aiScoreTemp = 0;
            int baseBet = std::min(BASE_BET, std::min(state.playerChips, state.aiChips));
            state.roundBet = baseBet;
            waitingForDoubleChoice = false;
            betStatusMessage = "Shared cards hidden. Click REVEAL to continue.";
            state.needRedraw = true;
        }
        else if (clickTitle(mx, my))
        {
            state.currentPhase = TITLE;
            waitingForDoubleChoice = false;
            betStatusMessage.clear();
            state.needRedraw = true;
        }
        break;
    }

    case REVEAL:
        if (waitingForDoubleChoice)
        {
            if (clickDouble(mx, my))
            {
                handlePlayerBetChoice(true);
            }
            else if (clickPass(mx, my))
            {
                handlePlayerBetChoice(false);
            }
        }
        else
        {
            if (clickRevealNext(mx, my))
            {
                if (state.revealedCommunityCards < (int)state.communityCards.size())
                {
                    revealNextCommunityCard();
                }
                else
                {
                    concludeRevealIfReady();
                }
            }
        }
        break;

    case RESULT:
        if (waitingForDoubleChoice)
        {
            if (clickDouble(mx, my))
            {
                int maxRaise = std::min(state.playerChips, state.aiChips);
                if (state.roundBet <= 0)
                {
                    state.roundBet = std::min(BASE_BET, maxRaise);
                }
                else
                {
                    state.roundBet = std::min(state.roundBet * 2, maxRaise);
                }
                settleRound();
            }
            else if (clickPass(mx, my))
            {
                settleRound();
            }
            break;
        }

        if (clickAgain(mx, my) && !waitingForDoubleChoice)
        {
            logic.dealCards(state);
            state.selectedIndices.clear();
            state.roundBet = 0;
            waitingForDoubleChoice = false;
            betStatusMessage.clear();
            state.currentPhase = PLAYER_TURN;
            state.needRedraw = true;
        }
        else if (clickTitle(mx, my))
        {
            state.currentPhase = TITLE;
            waitingForDoubleChoice = false;
            betStatusMessage.clear();
            state.needRedraw = true;
        }
        else if (clickQuit(mx, my))
        {
            state.running = false;
        }
        break;

    default:
        break;
    }
}
