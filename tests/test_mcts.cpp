#include "../src/mcts.h"
#include "../src/board.h"
#include <iostream>

int main() {
    Board b;
    MCTS mcts(100);   // 100 simulations per move
    int move = mcts.getBestMove(b);
    std::cout << "Best move from fresh board: " << move << std::endl;
    assert(move >= 0 && move <= 3);
    std::cout << "Test passed.\n";
    return 0;
}