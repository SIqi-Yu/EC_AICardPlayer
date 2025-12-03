#pragma once
#include <vector>
#include <string>
#include <utility>
#include <random>
#include "Hand.h"

class AIPlayer {
public:
    AIPlayer(const std::string& mode);

    std::vector<int> decideCardsToReplace(const Hand& h) const;

private:
    std::string mode;
    static constexpr int MONTE_CARLO_SAMPLES = 400;

    std::vector<int> decideCardsMonteCarlo(const Hand& h) const;
    std::vector<Card> BuildRemainingDeck(const std::vector<Card>& handCards) const;
    double EstimateSubsetValue(const std::vector<Card>& baseCards,
        const std::vector<int>& replaceIdx,
        const std::vector<Card>& pool,
        std::mt19937& rng) const;
};
