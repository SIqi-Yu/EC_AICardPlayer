# EC_AICardPlayer
Final Project for teamTBD

## User Guide

- Title Screen: Click `START` to play, `HOW TO PLAY` for the in-game rules overlay, or `QUIT` to exit. The right-side card emblem and background/intro art now render in the correct orientation.
- Opening Hand: You and the AI each receive 7 private cards. Click cards to toggle selection and press `DONE` to swap the selected ones once. Press `STAND` to keep your hand.
- Shared Cards & Betting: Three shared cards sit between players. Reveal them one by one with `REVEAL`/`SHOWDOWN`. After each reveal, choose `DOUBLE` or `PASS`; the AI reacts immediately.
- Advice: Toggle the `Advice` button (bottom-left) to see suggested swaps and flush/pair potential based on your current hand. It will gently pulse if you are idle.
- Results: At showdown, both hands are scored using standard poker rankings and the winner takes the current bet. Use `AGAIN` to re-deal or `TITLE` to return to the main menu.
- Sound/Visuals: Intro splash, textured table background, enlarged right-side emblem, and looping background music. Match-end popups play success (`purchase-success.wav`) or failure (`fail-234710.wav`) cues.

## AI Strategy

The computer player no longer relies on a remote API. Instead, its `AIPlayer` class uses a Monte Carlo approach:

1. Enumerate all subsets of the current 7 cards that the AI could replace (including the option of keeping everything).
2. For each subset, simulate drawing replacements from the remaining deck hundreds of times (default: 400 samples).
3. Evaluate the resulting hand using the same scoring function as the human player and compute the average score over all simulations.
4. Keep the subset whose estimated average score is highest, and replace those cards when the AI takes its turn.

This strategy makes the AI stronger without adding latency or external dependencies. You can tweak the number of Monte Carlo samples in `AIPlayer::MONTE_CARLO_SAMPLES` to trade accuracy for speed.

## Build & Run

### macOS (clang++)

```bash
clang++ -std=c++17 \
  main.cpp UI_Graphics.cpp UI.cpp GameLogic.cpp GameState.cpp \
  Card.cpp Deck.cpp Hand.cpp AIPlayer.cpp \
  fssimplewindowcpp.cpp fssimplewindowobjc.m \
  yssimplesound.cpp yssimplesound_macosx_objc.m \
  -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
  -framework AudioUnit -framework AudioToolbox -framework CoreAudio \
  -o SevenCardStrategyDuel
./SevenCardStrategyDuel
```

### Windows (Visual Studio)

- Open `FinalProjectGame.sln` in Visual Studio.
- Set the startup project to `FinalProjectGame` and build/run (Debug x64 or Win32 as needed).

### Assets

Ensure the executable can find these assets in the working directory:

- `AdobeStock_267112986.jpeg` (table background)
- `AdobeStock_512097593.jpeg` (intro splash)
- `AdobeStock_529389711.png` (title emblem)
- `las-vegas-407027.wav` (background music)
- `fail-234710.wav` (defeat cue)
- `purchase-success.wav` (victory cue)

## TODO

- ui优化，字体，按钮,文件结构整理,demo等
