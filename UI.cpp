#include "UI.h"
#include "Deck.h"
#include <iostream>
#include <algorithm>
#include <sstream>

static void PrintHand(const Hand& h)
{
    for (int i = 0; i < static_cast<int>(h.cards.size()); ++i)
    {
        const auto& c = h.cards[i];
        std::cout << "[" << i << ":" << c.rank << "," << c.suit << "] ";
    }
    std::cout << "\n";
}

UI::UI()
    : ai("Normal")
{
    state.playerScore = 0;
    state.aiScore = 0;
    state.playerScoreTemp = 0;
    state.aiScoreTemp = 0;
    state.currentPhase = TITLE;
    state.running = true;
}

void UI::mainLoop()
{
    std::cout << "===============================\n";
    std::cout << "  Seven-Card Strategy Duel (Text UI)\n";
    std::cout << "===============================\n";

    while (state.running)
    {
        std::cout << "Type 'start' to play, 'quit' to exit.\n> ";
        std::string cmd;
        if (!(std::cin >> cmd)) break;

        if (cmd == "quit")
        {
            break;
        }
        else if (cmd == "start")
        {
            logic.dealCards(state);
            state.selectedIndices.clear();
            handlePlayerTurn();
        }
    }
}

void UI::renderGame()
{
    std::cout << "\n--- Player Turn ---\n";
    std::cout << "Your hand:\n";
    PrintHand(state.playerHand);
    std::cout << "Current score: Player " << state.playerScore
        << "  -  AI " << state.aiScore << "\n";
    std::cout << "Commands:\n"
        << "  sel <i>      : select card index to replace\n"
        << "  done         : finish selection & replace\n"
        << "  stand        : do not replace, go to showdown\n"
        << "  show         : re-print your hand\n"
        << "  back         : return to title\n";
}

void UI::renderResult()
{
    std::cout << "\n===== Result =====\n";
    std::cout << "Your final hand:\n";
    PrintHand(state.playerHand);
    std::cout << "AI final hand:\n";
    PrintHand(state.aiHand);

    if (state.playerScoreTemp > state.aiScoreTemp)
    {
        std::cout << "You win this round!\n";
    }
    else if (state.playerScoreTemp < state.aiScoreTemp)
    {
        std::cout << "AI wins this round.\n";
    }
    else
    {
        std::cout << "This round is a tie.\n";
    }

    std::cout << "Total score: Player " << state.playerScore
        << "  -  AI " << state.aiScore << "\n";

    std::cout << "Commands:\n"
        << "  again  : play another round\n"
        << "  title  : back to title\n"
        << "  quit   : exit game\n";
}

void UI::handlePlayerTurn()
{
    state.currentPhase = PLAYER_TURN;
    state.selectedIndices.clear();

    std::string line;
    std::getline(std::cin, line); // �����һ�ζ�ȡ�Ļ���

    while (true)
    {
        renderGame();
        std::cout << "> ";
        if (!std::getline(std::cin, line)) return;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "show")
        {
            continue;
        }
        else if (cmd == "sel")
        {
            int idx;
            if (iss >> idx)
            {
                auto& v = state.selectedIndices;
                auto it = std::find(v.begin(), v.end(), idx);
                if (it == v.end())
                {
                    v.push_back(idx);
                }
                else
                {
                    v.erase(it);
                }
            }
        }
        else if (cmd == "done" || cmd == "stand")
        {
            // ��һ���ƶѸ����/AI ���ƣ��򻯴�����
            Deck deck;
            deck.shuffle();

            if (cmd == "done")
            {
                logic.replaceCards(state.playerHand, state.selectedIndices, deck);
            }
            state.selectedIndices.clear();

            auto aiIndices = ai.decideCardsToReplace(state.aiHand);
            logic.replaceCards(state.aiHand, aiIndices, deck);

            int sharedCount = static_cast<int>(state.communityCards.size());
            int pScore = logic.evaluateWithCommunity(state.playerHand, state.communityCards, sharedCount);
            int aScore = logic.evaluateWithCommunity(state.aiHand, state.communityCards, sharedCount);
            logic.updateScore(state, pScore, aScore);

            state.currentPhase = RESULT;

            // ����������ѭ��
            while (true)
            {
                renderResult();
                std::cout << "> ";
                if (!std::getline(std::cin, line)) return;
                if (line.empty()) continue;
                std::istringstream iss2(line);
                std::string cmd2;
                iss2 >> cmd2;

                if (cmd2 == "again")
                {
                    logic.dealCards(state);
                    state.selectedIndices.clear();
                    state.currentPhase = PLAYER_TURN;
                    break;
                }
                else if (cmd2 == "title")
                {
                    return; 
                }
                else if (cmd2 == "quit")
                {
                    state.running = false;
                    return;
                }
            }
        }
        else if (cmd == "back")
        {
            return; 
        }
        else
        {
            std::cout << "Unknown command.\n";
        }

        if (!state.running)
        {
            return;
        }
    }
}
