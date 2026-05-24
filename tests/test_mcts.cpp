#include "../src/board.h"
#include "../src/mcts.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <cstring>

// Applies a move int (0=Left, 1=Right, 2=Up, 3=Down) to a Board.
// Returns true if the move was legal (board changed), false otherwise.
bool applyMoveToBoard(Board& b, int move) {
    int before[4][4];
    std::memcpy(before, b.board, sizeof(before));

    switch (move) {
        case 0: b.swipeLeft();  break;
        case 1: b.swipeRight(); break;
        case 2: b.swipeUP();    break;
        case 3: b.swipeDown();  break;
        default: return false;
    }

    return std::memcmp(before, b.board, sizeof(before)) != 0;
}


// Plays one full game using MCTS with `simulations` rollouts per move.
// Returns the final game score.
int playMCTSGame(int simulations) {
    Board b;
    MCTS mcts(simulations);

    while (!b.isGameOver()) {
        int move = mcts.getBestMove(b);

        // TODO 1: apply the chosen move to b, then spawn a tile.
        // Use applyMoveToBoard(b, move) and b.spawnTile().
        // If applyMoveToBoard returns false something is wrong (MCTS picked
        // an illegal move) — break out of the loop so we don't infinite-loop.
        if (!applyMoveToBoard(b, move)) break;
        b.spawnTile();

    }

    return b.score;
}


// Plays one full game picking moves uniformly at random from legal moves.
// Returns the final game score.
int playRandomGame(std::mt19937& rng) {
    Board b;

   while (!b.isGameOver()) {
        std::vector<int> legal;
        for (int m = 0; m < 4; ++m) {
            int tmp[4][4];
            std::memcpy(tmp, b.board, sizeof(tmp));
            Board test;
            std::memcpy(test.board, tmp, sizeof(test.board));
            switch (m) {
                case 0: test.swipeLeft();  break;
                case 1: test.swipeRight(); break;
                case 2: test.swipeUP();    break;
                case 3: test.swipeDown();  break;
            }
            if (std::memcmp(test.board, b.board, sizeof(test.board)) != 0) {
                legal.push_back(m);
            }
        }
        if (legal.empty()) break;
        int move = legal[std::uniform_int_distribution<int>(0, legal.size() - 1)(rng)];
        applyMoveToBoard(b, move);
        b.spawnTile();
    }
    return b.score;
}


int main() {
    const int N_GAMES = 10;
    std::mt19937 rng(42);  // fixed seed for reproducibility

    std::vector<int> random_scores;
    std::vector<int> mcts100_scores;
    std::vector<int> mcts1000_scores;

    std::cout << "Playing " << N_GAMES << " random games...\n";
    for (int i = 0; i < N_GAMES; ++i) {
        int s = playRandomGame(rng);
        random_scores.push_back(s);
        std::cout << "  random[" << i << "] = " << s << "\n";
    }

    std::cout << "\nPlaying " << N_GAMES << " MCTS(100) games...\n";
    for (int i = 0; i < N_GAMES; ++i) {
        int s = playMCTSGame(100);
        mcts100_scores.push_back(s);
        std::cout << "  mcts100[" << i << "] = " << s << "\n";
    }

    std::cout << "\nPlaying " << N_GAMES << " MCTS(1000) games...\n";
    for (int i = 0; i < N_GAMES; ++i) {
        int s = playMCTSGame(1000);
        mcts1000_scores.push_back(s);
        std::cout << "  mcts1000[" << i << "] = " << s << "\n";
    }

    // TODO 3: compute and print the averages for each of the three groups.
    // Use std::accumulate from <numeric>.
    // Expected pattern: random < mcts100 < mcts1000.
    // If that pattern doesn't hold, there's a bug.
    double avg_random = std::accumulate(random_scores.begin(), random_scores.end(), 0.0) / N_GAMES;
    double avg_mcts100 = std::accumulate(mcts100_scores.begin(), mcts100_scores.end(), 0.0) / N_GAMES;
    double avg_mcts1000 = std::accumulate(mcts1000_scores.begin(), mcts1000_scores.end(), 0.0) / N_GAMES;

    std::cout << "  Random:     " << avg_random   << "\n";
    std::cout << "  MCTS(100):  " << avg_mcts100  << "\n";
    std::cout << "  MCTS(1000): " << avg_mcts1000 << "\n";

    if (avg_random < avg_mcts100 && avg_mcts100 < avg_mcts1000)
        std::cout << "\n[OK] Expected ordering: random < mcts100 < mcts1000\n";
    else
        std::cout << "\n[WARN] Ordering not as expected — possible bug.\n";


    return 0;
}