#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <memory>
#include "board.h"
#include "mcts.h"
#include "network.h"

// Color mapping for 2048 tiles using ANSI escape sequences
std::string getTileColor(int value) {
    switch (value) {
        case 0:    return "\033[90m"; // Gray
        case 2:    return "\033[36m"; // Cyan
        case 4:    return "\033[34m"; // Blue
        case 8:    return "\033[32m"; // Green
        case 16:   return "\033[33m"; // Yellow
        case 32:   return "\033[35m"; // Magenta
        case 64:   return "\033[31m"; // Red
        case 128:  return "\033[96m"; // Bright Cyan
        case 256:  return "\033[94m"; // Bright Blue
        case 512:  return "\033[92m"; // Bright Green
        case 1024: return "\033[93m"; // Bright Yellow
        case 2048: return "\033[95m"; // Bright Magenta
        default:   return "\033[91m"; // Bright Red (for > 2048)
    }
}

void printBoard(const Board& b) {
    std::cout << "\033[H\033[2J"; // Clear screen and reset cursor
    std::cout << "\033[1;35m====================================\033[0m\n";
    std::cout << "\033[1;36m           TILE MAXXING (2048)      \033[0m\n";
    std::cout << "\033[1;35m====================================\033[0m\n";
    std::cout << " Score: \033[1;32m" << b.score << "\033[0m\n\n";

    for (int r = 0; r < 4; ++r) {
        std::cout << "  +------+------+------+------+\n  |";
        for (int c = 0; c < 4; ++c) {
            int val = b.board[r][c];
            std::string color = getTileColor(val);
            if (val == 0) {
                std::cout << "      \033[0m|";
            } else {
                std::cout << color << std::setw(5) << val << " \033[0m|";
            }
        }
        std::cout << "\n";
    }
    std::cout << "  +------+------+------+------+\n\n";
    std::cout << " Controls: \033[1;33mW\033[0m (Up), \033[1;33mA\033[0m (Left), \033[1;33mS\033[0m (Down), \033[1;33mD\033[0m (Right)\n";
    std::cout << "           \033[1;32mM\033[0m (MCTS AI Best Move), \033[1;31mQ\033[0m (Quit)\n";
}

bool applyMove(Board& b, char move) {
    int before[4][4];
    std::memcpy(before, b.board, sizeof(before));
    
    switch (move) {
        case 'w': case 'W': b.swipeUP(); break;
        case 's': case 'S': b.swipeDown(); break;
        case 'a': case 'A': b.swipeLeft(); break;
        case 'd': case 'D': b.swipeRight(); break;
        default: return false;
    }
    
    return std::memcmp(before, b.board, sizeof(before)) != 0;
}

std::string getMoveName(int move) {
    switch (move) {
        case 0: return "Left (A)";
        case 1: return "Right (D)";
        case 2: return "Up (W)";
        case 3: return "Down (S)";
        default: return "Unknown";
    }
}

char directionToChar(int dir) {
    switch (dir) {
        case 0: return 'a';
        case 1: return 'd';
        case 2: return 'w';
        case 3: return 's';
        default: return ' ';
    }
}

int main() {
    Board b;
    std::unique_ptr<NetworkInference> net;
    
    // Try to load the ONNX network model if available
    try {
        net = std::make_unique<NetworkInference>("./model/tile_value_net.onnx");
    } catch (...) {
        // Safe fallback to pure rollout MCTS if model does not exist or fails to load
    }

    std::string message = "Welcome to TileMaxxing! Make your first move.";

    while (!b.isGameOver()) {
        printBoard(b);
        std::cout << "\n Status: " << message << "\n\n";
        std::cout << " Enter action: ";
        
        char choice;
        std::cin >> choice;
        
        if (choice == 'q' || choice == 'Q') {
            std::cout << "Thanks for playing!\n";
            return 0;
        }
        
        if (choice == 'm' || choice == 'M') {
            message = "MCTS AI is thinking...";
            printBoard(b);
            std::cout << "\n Status: \033[1;36m" << message << "\033[0m\n";
            
            // Run MCTS with 200 rollouts
            MCTS mcts(200, net.get());
            MCTS::MoveResult res = mcts.getBestMove(b);
            
            char mcts_char = directionToChar(res.move);
            if (applyMove(b, mcts_char)) {
                b.spawnTile();
                message = "AI played: \033[1;32m" + getMoveName(res.move) + "\033[0m (Eval: " + std::to_string(res.eval) + ")";
            } else {
                message = "AI tried to play " + getMoveName(res.move) + " but it was illegal.";
            }
        } else {
            if (applyMove(b, choice)) {
                b.spawnTile();
                message = "Successfully swiped!";
            } else {
                message = "\033[1;31mInvalid/Illegal move!\033[0m Try again.";
            }
        }
    }

    printBoard(b);
    std::cout << "\n\033[1;31mGAME OVER!\033[0m Final Score: \033[1;32m" << b.score << "\033[0m\n";
    return 0;
}
