// include/AIPlayer.h
#pragma once

#include <string>
#include <vector>
#include "Hand.h"

// Simple AI player that decides which cards to replace.
class AIPlayer {
public:
    AIPlayer(const std::string& mode = "Normal");

    // Decide indices of cards to replace based on current hand and mode.
    std::vector<int> decideCardsToReplace(const Hand& h);

    void setMode(const std::string& mode);

private:
    std::string mode;
};
