// src/AIPlayer.cpp
#include "AIPlayer.h"
#include <algorithm>

AIPlayer::AIPlayer(const std::string& m)
    : mode(m) {
}

void AIPlayer::setMode(const std::string& m) {
    mode = m;
}

// Very naive heuristic:
// - sort indices by rank ascending
// - replace up to 2 lowest-ranked cards
std::vector<int> AIPlayer::decideCardsToReplace(const Hand& h) {
    const int MAX_REPLACE = 2;
    std::vector<int> indices(h.cards.size());
    for (int i = 0; i < (int)h.cards.size(); ++i) {
        indices[i] = i;
    }

    std::sort(indices.begin(), indices.end(),
              [&h](int a, int b) {
                  return h.cards[a].rank < h.cards[b].rank;
              });

    std::vector<int> toReplace;
    for (int i = 0; i < (int)indices.size() && (int)toReplace.size() < MAX_REPLACE; ++i) {
        toReplace.push_back(indices[i]);
    }
    return toReplace;
}
