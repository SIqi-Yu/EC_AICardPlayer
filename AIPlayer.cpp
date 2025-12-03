#include "AIPlayer.h"
#include <algorithm>
#include <random>

AIPlayer::AIPlayer(const std::string& mode)
    : mode(mode)
{
}


std::vector<int> AIPlayer::decideCardsToReplace(const Hand& h) const
{
    return decideCardsMonteCarlo(h);
}

std::vector<int> AIPlayer::decideCardsMonteCarlo(const Hand& h) const
{
    std::vector<int> bestIndices;
    const auto& cards = h.cards;
    const int n = static_cast<int>(cards.size());
    if (n == 0)
    {
        return bestIndices;
    }

    auto pool = BuildRemainingDeck(cards);
    if (pool.empty())
    {
        return bestIndices;
    }

    std::mt19937 rng(std::random_device{}());

    Hand baseHand;
    baseHand.cards = cards;
    double bestScore = static_cast<double>(baseHand.evaluateHand());

    const int totalMask = 1 << n;
    for (int mask = 1; mask < totalMask; ++mask)
    {
        std::vector<int> subset;
        subset.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            if (mask & (1 << i))
            {
                subset.push_back(i);
            }
        }
        double estimated = EstimateSubsetValue(cards, subset, pool, rng);
        if (estimated > bestScore + 1e-6)
        {
            bestScore = estimated;
            bestIndices = subset;
        }
    }

    return bestIndices;
}

std::vector<Card> AIPlayer::BuildRemainingDeck(const std::vector<Card>& handCards) const
{
    std::vector<Card> pool;
    pool.reserve(52);
    for (int s = 0; s < 4; ++s)
    {
        for (int r = 2; r <= 14; ++r)
        {
            Card c;
            c.rank = r;
            c.suit = s;
            pool.push_back(c);
        }
    }

    for (const auto& card : handCards)
    {
        auto it = std::find_if(pool.begin(), pool.end(), [&](const Card& c)
            {
                return c.rank == card.rank && c.suit == card.suit;
            });
        if (it != pool.end())
        {
            pool.erase(it);
        }
    }
    return pool;
}

double AIPlayer::EstimateSubsetValue(const std::vector<Card>& baseCards,
    const std::vector<int>& replaceIdx,
    const std::vector<Card>& pool,
    std::mt19937& rng) const
{
    if (replaceIdx.empty())
    {
        Hand h;
        h.cards = baseCards;
        return static_cast<double>(h.evaluateHand());
    }
    if (pool.size() < replaceIdx.size())
    {
        return 0.0;
    }

    double total = 0.0;
    std::vector<Card> trialCards;
    trialCards.reserve(baseCards.size());
    std::vector<Card> shuffledPool = pool;

    for (int sample = 0; sample < MONTE_CARLO_SAMPLES; ++sample)
    {
        trialCards = baseCards;
        std::shuffle(shuffledPool.begin(), shuffledPool.end(), rng);
        for (size_t i = 0; i < replaceIdx.size(); ++i)
        {
            trialCards[replaceIdx[i]] = shuffledPool[i];
        }
        Hand temp;
        temp.cards = trialCards;
        total += temp.evaluateHand();
    }
    return total / static_cast<double>(MONTE_CARLO_SAMPLES);
}
