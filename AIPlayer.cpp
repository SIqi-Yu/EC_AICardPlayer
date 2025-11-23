#include "AIPlayer.h"
#include <algorithm>

AIPlayer::AIPlayer(const std::string& mode)
    : mode(mode)
{
}

/*
 简单 AI：
 - 保留所有组成对子/三条/四条的牌
 - 如果某个花色数量 ≥4，优先保留该花色的牌（冲同花）
 - 其他牌全部换掉
*/
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

    // 1) 先保留所有属于对子及以上的牌
    for (int i = 0; i < n; ++i)
    {
        if (rankCount[cards[i].rank] >= 2)
        {
            keep[i] = true;
        }
    }

    // 2) 再尝试冲同花：找到数量最多的花色
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

    // 3) 未被标记为 keep 的牌，全部丢掉
    for (int i = 0; i < n; ++i)
    {
        if (!keep[i])
        {
            toReplace.push_back(i);
        }
    }

    return toReplace;
}
