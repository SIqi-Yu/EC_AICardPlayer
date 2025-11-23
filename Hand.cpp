#include "Hand.h"
#include <algorithm>

// 辅助：从高到低排序
static bool CompareRankDesc(const Card& a, const Card& b)
{
    return a.rank > b.rank;
}

void Hand::replaceCard(int idx, const Card& c)
{
    if (0 <= idx && idx < static_cast<int>(cards.size()))
    {
        cards[idx] = c;
    }
}

/*
 7 张牌评估：
 返回一个整数 score，数值越大牌力越强。
 大致方案：
   category(0~8) * 1,000,000 + high1 * 10,000 + high2 * 100 + high3

 category:
   8 = Straight Flush
   7 = Four of a Kind
   6 = Full House
   5 = Flush
   4 = Straight
   3 = Three of a Kind
   2 = Two Pair
   1 = One Pair
   0 = High Card
*/
int Hand::evaluateHand() const
{
    if (cards.empty())
    {
        return 0;
    }

    int rankCount[15] = { 0 };  // 2~14
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

    // ----- 是否同花 -----
    bool hasFlush = false;
    int flushSuit = -1;
    for (int s = 0; s < 4; ++s)
    {
        if (suitCount[s] >= 5)
        {
            hasFlush = true;
            flushSuit = s;
            break;
        }
    }

    // ----- 是否顺子（全局） -----
    bool present[15] = { false };  // 2~14，额外使用 1 表示 A 作 1
    for (int r = 2; r <= 14; ++r)
    {
        if (rankCount[r] > 0)
        {
            present[r] = true;
        }
    }
    present[1] = present[14]; // A2345

    auto findStraightHigh = [&](const bool useFlush, int suit) -> int
        {
            bool used[15] = { false };
            if (useFlush)
            {
                for (const auto& c : cards)
                {
                    if (c.suit == suit && c.rank >= 2 && c.rank <= 14)
                    {
                        used[c.rank] = true;
                    }
                }
                used[1] = used[14];
            }
            else
            {
                for (int r = 1; r <= 14; ++r)
                {
                    used[r] = present[r];
                }
            }

            // 从高到低找 5 连
            for (int r = 14; r >= 5; --r)
            {
                bool ok = true;
                for (int k = 0; k < 5; ++k)
                {
                    if (!used[r - k])
                    {
                        ok = false;
                        break;
                    }
                }
                if (ok)
                {
                    return r == 5 && used[14] ? 5 : r;
                }
            }
            return 0;
        };

    int straightHigh = findStraightHigh(false, -1);
    int straightFlushHigh = (hasFlush ? findStraightHigh(true, flushSuit) : 0);

    // ----- 统计牌型：4 条、3 条、对子 -----
    int fourRank = 0;
    int threeRank1 = 0, threeRank2 = 0; // 可能有两个三条
    int pair1 = 0, pair2 = 0;

    for (int r = 14; r >= 2; --r)
    {
        if (rankCount[r] == 4)
        {
            fourRank = r;
        }
        else if (rankCount[r] == 3)
        {
            if (threeRank1 == 0)
            {
                threeRank1 = r;
            }
            else if (threeRank2 == 0)
            {
                threeRank2 = r;
            }
        }
        else if (rankCount[r] == 2)
        {
            if (pair1 == 0)
            {
                pair1 = r;
            }
            else if (pair2 == 0)
            {
                pair2 = r;
            }
        }
    }

    // ----- 具体评分 -----
    int category = 0;
    int hi1 = 0, hi2 = 0, hi3 = 0;

    // 8. 同花顺
    if (straightFlushHigh > 0)
    {
        category = 8;
        hi1 = straightFlushHigh;
    }
    // 7. 四条
    else if (fourRank > 0)
    {
        category = 7;
        hi1 = fourRank;
        for (int r = 14; r >= 2; --r)
        {
            if (r != fourRank && rankCount[r] > 0)
            {
                hi2 = r;
                break;
            }
        }
    }
    // 6. 满堂红（葫芦）
    else if (threeRank1 > 0 && (threeRank2 > 0 || pair1 > 0))
    {
        category = 6;
        hi1 = threeRank1;
        hi2 = (threeRank2 > 0 ? threeRank2 : pair1);
    }
    // 5. 同花
    else if (hasFlush)
    {
        category = 5;
        // 取该花色最高的 5 张作 tiebreak
        std::vector<int> rks;
        for (const auto& c : cards)
        {
            if (c.suit == flushSuit)
            {
                rks.push_back(c.rank);
            }
        }
        std::sort(rks.begin(), rks.end(), std::greater<int>());
        if (!rks.empty()) hi1 = rks[0];
        if (rks.size() > 1) hi2 = rks[1];
        if (rks.size() > 2) hi3 = rks[2];
    }
    // 4. 顺子
    else if (straightHigh > 0)
    {
        category = 4;
        hi1 = straightHigh;
    }
    // 3. 三条
    else if (threeRank1 > 0)
    {
        category = 3;
        hi1 = threeRank1;

        for (int r = 14; r >= 2; --r)
        {
            if (r != threeRank1 && rankCount[r] > 0)
            {
                if (hi2 == 0) hi2 = r;
                else { hi3 = r; break; }
            }
        }
    }
    // 2. 两对
    else if (pair1 > 0 && pair2 > 0)
    {
        category = 2;
        hi1 = pair1;
        hi2 = pair2;
        for (int r = 14; r >= 2; --r)
        {
            if (r != pair1 && r != pair2 && rankCount[r] > 0)
            {
                hi3 = r;
                break;
            }
        }
    }
    // 1. 一对
    else if (pair1 > 0)
    {
        category = 1;
        hi1 = pair1;
        int cnt = 0;
        for (int r = 14; r >= 2; --r)
        {
            if (r != pair1 && rankCount[r] > 0)
            {
                if (cnt == 0) hi2 = r;
                else if (cnt == 1) { hi3 = r; break; }
                ++cnt;
            }
        }
    }
    // 0. 高牌
    else
    {
        category = 0;
        std::vector<int> rks;
        for (int r = 14; r >= 2; --r)
        {
            for (int k = 0; k < rankCount[r]; ++k)
            {
                rks.push_back(r);
            }
        }
        if (!rks.empty()) hi1 = rks[0];
        if (rks.size() > 1) hi2 = rks[1];
        if (rks.size() > 2) hi3 = rks[2];
    }

    int score = category * 1000000 + hi1 * 10000 + hi2 * 100 + hi3;
    return score;
}
