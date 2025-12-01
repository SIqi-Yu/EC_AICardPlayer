#pragma once
#include <vector>
#include <string>
#include "Hand.h"

class AIPlayer {
public:
    AIPlayer(const std::string& mode);

    std::vector<int> decideCardsToReplace(const Hand& h) const;

private:
    std::string mode;
};
