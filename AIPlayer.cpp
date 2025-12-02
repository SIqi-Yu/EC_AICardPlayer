#include "AIPlayer.h"
#include <algorithm>

AIPlayer::AIPlayer(const std::string& mode)
    : mode(mode)
{
}


std::vector<int> AIPlayer::decideCardsToReplace(const Hand& h) const
{
    std::vector<int> toReplace;
    const auto& cards = h.cards;
    const int n = static_cast<int>(cards.size());
    if (n == 0)
    {
        return toReplace;
    }

    int rankCount[15] = { 0 }; // 2~14
    int suitCount[4] = { 0 };  // 0~3

    for (const auto& c : cards)
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

    bool keep[7] = { false,false,false,false,false,false,false };


    for (int i = 0; i < n; ++i)
    {
        if (rankCount[cards[i].rank] >= 2)
        {
            keep[i] = true;
        }
    }


    int bestSuit = 0;
    int bestSuitCount = 0;
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
        for (int i = 0; i < n; ++i)
        {
            if (cards[i].suit == bestSuit)
            {
                keep[i] = true;
            }
        }
    }


    for (int i = 0; i < n; ++i)
    {
        if (!keep[i])
        {
            toReplace.push_back(i);
        }
    }

    return toReplace;
}
