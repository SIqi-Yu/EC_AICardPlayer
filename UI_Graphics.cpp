#include "UI_Graphics.h"
#include "ysglfontdata.h"
#include "Deck.h"
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <vector>

// 修复 Windows 没有 M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---- 一些常量，方便统一改布局 ----
static const int WINDOW_W = 1200;
static const int WINDOW_H = 700;

static const int CARD_W = 110;
static const int CARD_H = 160;
static const int CARD_GAP = 20;   // 牌与牌之间的水平间隔

// 根据窗口宽度，把 7 张牌水平居中
static inline int PlayerBaseX()
{
    const int totalW = 7 * CARD_W + 6 * CARD_GAP;
    return (WINDOW_W - totalW) / 2;
}

static inline int PlayerBaseY()
{
    return 140;
}

static inline int AIBaseX()
{
    return PlayerBaseX();
}

static inline int AIBaseY()
{
    return 380;
}

static const int BUTTON_WIDTH = 220;
static const int BUTTON_HEIGHT = 40;
static const int BUTTON_GAP = 8;

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

// 小号字体
static void DrawSmallText(int x, int y, const std::string& s)
{
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap12x16(s.c_str());
}

// 花色小图标（用简单几何图形近似扑克牌花色）
static void DrawSuitSymbol(int cx, int cy, int suit)
{
    // suit: 0=♠,1=♥,2=♣,3=♦
    switch (suit)
    {
    case 0: // ♠ 黑桃
        glColor3ub(0, 0, 0);
        break;
    case 1: // ♥ 红心
        glColor3ub(220, 40, 40);
        break;
    case 2: // ♣ 梅花
        glColor3ub(0, 0, 0);
        break;
    case 3: // ♦ 方块
        glColor3ub(220, 40, 40);
        break;
    }

    if (suit == 0) // ♠：一个上圆 + 下三角
    {
        int r = 12;
        glBegin(GL_TRIANGLE_FAN);          // 上半部圆
        glVertex2i(cx, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * M_PI;
            int x = cx + (int)(r * std::cos(ang));
            int y = cy - 5 - (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLES);             // 下三角
        glVertex2i(cx - r, cy - 5);
        glVertex2i(cx + r, cy - 5);
        glVertex2i(cx, cy + r + 8);
        glEnd();
    }
    else if (suit == 1) // ♥：两个圆 + 下三角
    {
        int r = 10;
        glBegin(GL_TRIANGLE_FAN);          // 左圆
        glVertex2i(cx - r / 2, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx - r / 2 + (int)(r * std::cos(ang));
            int y = cy - 5 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);          // 右圆
        glVertex2i(cx + r / 2, cy - 5);
        for (int i = 0; i <= 20; ++i)
        {
            double ang = i / 20.0 * 2 * M_PI;
            int x = cx + r / 2 + (int)(r * std::cos(ang));
            int y = cy - 5 + (int)(r * std::sin(ang));
            glVertex2i(x, y);
        }
        glEnd();

        glBegin(GL_TRIANGLES);             // 下尖
        glVertex2i(cx - r - 2, cy);
        glVertex2i(cx + r + 2, cy);
        glVertex2i(cx, cy + r + 10);
        glEnd();
    }
    else if (suit == 2) // ♣：三个圆 + 柄
    {
        int r = 8;
        // 上圆
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
        // 左圆
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
        // 右圆
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

        // 柄
        glBegin(GL_TRIANGLES);
        glVertex2i(cx - 4, cy + r + 3);
        glVertex2i(cx + 4, cy + r + 3);
        glVertex2i(cx, cy + r + 12);
        glEnd();
    }
    else if (suit == 3) // ♦：菱形
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

// 把 rank 转成 “A,K,Q,J,10,9,...”
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

// 画单张牌
static void DrawCardAt(int x, int y, const Card& c, bool selected)
{
    // 阴影
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(x + 4, y + 4);
    glVertex2i(x + 4 + CARD_W, y + 4);
    glVertex2i(x + 4 + CARD_W, y + 4 + CARD_H);
    glVertex2i(x + 4, y + 4 + CARD_H);
    glEnd();

    // 白色卡面
    glColor3ub(245, 245, 245);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();

    // 黑色边框
    glColor3ub(40, 40, 40);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + CARD_W, y);
    glVertex2i(x + CARD_W, y + CARD_H);
    glVertex2i(x, y + CARD_H);
    glEnd();

    // 选中红框
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

    // rank 文本（左上角）
    std::string rStr = RankToString(c.rank);
    glColor3ub(0, 0, 0);
    if (c.suit == 1 || c.suit == 3)  // 红色花色
    {
        glColor3ub(200, 40, 40);
    }
    DrawSmallText(x + 8, y + 20, rStr);

    // suit 图标（中心位置）
    int cx = x + CARD_W / 2;
    int cy = y + CARD_H / 2;
    DrawSuitSymbol(cx, cy, c.suit);
}


// =================== UI_Graphics 成员实现 ===================

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
    if (YSOK == backgroundMusic.LoadWav("las-vegas-407027.wav"))
    {
        backgroundLoaded = true;
    }
    else
    {
        printf("Failed to load background music.\n");
    }
}

UI_Graphics::~UI_Graphics()
{
    if (backgroundLoaded)
    {
        soundPlayer.Stop(backgroundMusic);
    }
    soundPlayer.End();
}

void UI_Graphics::mainLoop()
{
    FsOpenWindow(100, 100, WINDOW_W, WINDOW_H, 1, "Seven-Card Strategy Duel");
    glClearColor(0, 0, 0, 0);
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
            state.needRedraw = false;
        }

        FsSleep(10);
    }
}


// ========== 绘制相关 ==========

void UI_Graphics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (state.currentPhase)
    {
    case TITLE:
        drawTitle();
        break;
    case PLAYER_TURN:
        drawPlayerHand();
        drawButtons();
        break;
    case AI_TURN:
        drawPlayerHand();
        drawButtons();
        break;
    case RESULT:
        drawPlayerHand();
        drawAIHand();
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

void UI_Graphics::drawTitle()
{
    // 背景稍微灰一点
    glColor3ub(30, 30, 30);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_W, 0);
    glVertex2i(WINDOW_W, WINDOW_H);
    glVertex2i(0, WINDOW_H);
    glEnd();

    drawText(330, 180, "Seven-Card Strategy Duel");
    DrawSmallText(400, 240, "Click START to play, or QUIT to exit.");

    auto drawTitleButton = [&](int y, const char* label)
    {
        int bx = 430, bw = 200, bh = 60;
        glColor3ub(80, 80, 80);
        glBegin(GL_QUADS);
        glVertex2i(bx, y);
        glVertex2i(bx + bw, y);
        glVertex2i(bx + bw, y + bh);
        glVertex2i(bx, y + bh);
        glEnd();

        glColor3ub(255, 255, 255);
        DrawSmallText(bx + 50, y + 35, label);
    };

    drawTitleButton(300, "START");
    drawTitleButton(380, "HOW TO PLAY");
    drawTitleButton(460, "QUIT");

    if (showInstructions)
    {
        drawInstructionsOverlay();
    }
}

void UI_Graphics::drawPlayerHand()
{
    // 背景
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_W, 0);
    glVertex2i(WINDOW_W, WINDOW_H);
    glVertex2i(0, WINDOW_H);
    glEnd();

    // 标题文字
    drawText(80, 80, "Your Hand:");

    // 分数显示在右上角
    char scoreBuf[128];
    std::snprintf(scoreBuf, sizeof(scoreBuf),
        "Total Score  Player %d : AI %d",
        state.playerScore, state.aiScore);
    DrawSmallText(800, 60, scoreBuf);

    // 画玩家 7 张牌
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
}

void UI_Graphics::drawAIHand()
{
    glColor3ub(255, 0, 0);
    DrawSmallText(80, AIBaseY() - 20, "AI Final Hand:");

    int baseX = AIBaseX();
    int baseY = AIBaseY();

    for (int i = 0; i < (int)state.aiHand.cards.size(); ++i)
    {
        int x = baseX + i * (CARD_W + CARD_GAP);
        int y = baseY;

        DrawCardAt(x, y, state.aiHand.cards[i], false);
    }

    // 显示本局得分
    char roundBuf[128];
    std::snprintf(roundBuf, sizeof(roundBuf),
        "Last Round  P = %d   AI = %d",
        state.playerScoreTemp, state.aiScoreTemp);
    glColor3ub(255, 0, 0);
    DrawSmallText(80, AIBaseY() + CARD_H + 40, roundBuf);
}

void UI_Graphics::drawButtons()
{
    int bx = ButtonsBaseX();

    auto drawButton = [&](int row, const char* label)
    {
        int by = ButtonRowY(row);
        glColor3ub(50, 50, 50);
        glBegin(GL_QUADS);
        glVertex2i(bx, by);
        glVertex2i(bx + BUTTON_WIDTH, by);
        glVertex2i(bx + BUTTON_WIDTH, by + BUTTON_HEIGHT);
        glVertex2i(bx, by + BUTTON_HEIGHT);
        glEnd();

        glColor3ub(255, 255, 255);
        DrawSmallText(bx + BUTTON_WIDTH / 2 - 25, by + BUTTON_HEIGHT / 2 + 5, label);
    };

    if (state.currentPhase == PLAYER_TURN)
    {
        drawButton(0, "DONE");
        drawButton(1, "STAND");
        drawButton(2, "TITLE");
    }
    else if (state.currentPhase == RESULT)
    {
        drawButton(0, "AGAIN");
        drawButton(1, "TITLE");
        drawButton(2, "QUIT");
    }
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
    drawText(WINDOW_W / 2 - 160, 120, "Game Instructions");

    std::vector<std::string> lines = {
        "Both you and the AI draw 7 cards.",
        "Click on cards to select them, then press DONE to replace them.",
        "Use STAND if you want to lock your current hand instead of swapping.",
        "AI exchanges cards automatically once you finalize your choice.",
        "The better poker hand wins the round, and the score is tracked below."
    };
    glColor3ub(220, 220, 220);
    int textY = 180;
    for (const auto& line : lines)
    {
        DrawSmallText(120, textY, line);
        textY += 32;
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
    DrawSmallText(bx + BUTTON_WIDTH / 2 - 35, by + BUTTON_HEIGHT / 2 + 5, "BACK");
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


// ========== 输入处理 ==========

bool UI_Graphics::inside(int mx, int my, int x, int y, int w, int h)
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
    // 标题界面 START 按钮范围
    return inside(mx, my, 430, 300, 200, 60);
}

bool UI_Graphics::clickQuit(int mx, int my)
{
    if (state.currentPhase == TITLE)
    {
        return inside(mx, my, 430, 460, 200, 60);
    }
    // 结果界面右侧 QUIT
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
    // RESULT 中的 TITLE
    return inside(mx, my, ButtonsBaseX(), ButtonRowY(1), BUTTON_WIDTH, BUTTON_HEIGHT);
}

void UI_Graphics::processInput()
{
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

    switch (state.currentPhase)
    {
    case TITLE:
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
            logic.dealCards(state);
            state.selectedIndices.clear();
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

            int pScore = logic.evaluate(state.playerHand);
            int aScore = logic.evaluate(state.aiHand);
            logic.updateScore(state, pScore, aScore);

            state.currentPhase = RESULT;
            state.needRedraw = true;
        }
        else if (clickStand(mx, my))
        {
            Deck deck;
            deck.shuffle();

            auto aiIndices = ai.decideCardsToReplace(state.aiHand);
            logic.replaceCards(state.aiHand, aiIndices, deck);

            int pScore = logic.evaluate(state.playerHand);
            int aScore = logic.evaluate(state.aiHand);
            logic.updateScore(state, pScore, aScore);

            state.currentPhase = RESULT;
            state.needRedraw = true;
        }
        else if (clickTitle(mx, my))
        {
            state.currentPhase = TITLE;
            state.needRedraw = true;
        }
        break;
    }

    case RESULT:
        if (clickAgain(mx, my))
        {
            logic.dealCards(state);
            state.selectedIndices.clear();
            state.currentPhase = PLAYER_TURN;
            state.needRedraw = true;
        }
        else if (clickTitle(mx, my))
        {
            state.currentPhase = TITLE;
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
