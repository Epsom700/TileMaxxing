#include "data_capture.h"
#include "board.h"
#include "mcts.h"
#include <fstream>
#include <cstring>

static void writeBoardJSON(std::ofstream& f, const int board[4][4]) {
    f << "[";
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            f << board[i][j];
            if (!(i == 3 && j == 3)) f << ",";
        }
    }
    f << "]";
}

int playAndLog(int simulations, std::ofstream& file, int game_id) {
    Board b;
    MCTS mcts(simulations);
    int move_idx = 0;

    while (!b.isGameOver()) {
        int snap[4][4];
        std::memcpy(snap, b.board, sizeof(snap));
        int score_before = b.score;

        MCTS::MoveResult r = mcts.getBestMove(b);

        switch (r.move) {
            case 0: b.swipeLeft();  break;
            case 1: b.swipeRight(); break;
            case 2: b.swipeUP();    break;
            case 3: b.swipeDown();  break;
        }
        b.spawnTile();
        int score_after = b.score;

        file << "{\"game_id\":" << game_id
             << ",\"move_idx\":" << move_idx
             << ",\"board\":";
        writeBoardJSON(file, snap);
        file << ",\"eval\":" << r.eval
             << ",\"move\":" << r.move
             << ",\"score_before\":" << score_before
             << ",\"score_after\":" << score_after
             << "}\n";

        ++move_idx;
    }

    return b.score;
}
