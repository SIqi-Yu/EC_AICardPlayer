#include "Deck.h"
#include <algorithm>
#include <random>
#include <ctime>

Deck::Deck()
{
    cards.clear();
    cards.reserve(52);

    // rank: 2~14 (A = 14), suit: 0~3
    for (int s = 0; s < 4; ++s)
    {
        for (int r = 2; r <= 14; ++r)
        {
            Card c;
            c.rank = r;
            c.suit = s;
            cards.push_back(c);
        }
    }

    index = 0;
    shuffle();
}

void Deck::shuffle()
{
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::shuffle(cards.begin(), cards.end(), rng);
    index = 0;
}

Card Deck::draw()
{
    if (index < static_cast<int>(cards.size()))
    {
        return cards[index++];
    }
    // 如果意外用完牌堆，返回一个“无效牌”，调用方一般不会真正用到
    Card c{};
    c.rank = 0;
    c.suit = 0;
    return c;
}

bool Deck::empty() const
{
    return index >= static_cast<int>(cards.size());
}
