#include <cassert>
#include <iostream>
#include "../src/board.h"

void test_board_initialization() {
    Board b;
    assert(b.score == 0);
    assert(b.gameOver == false);
    
    // Check that board has exactly 2 tiles after initialization
    int tile_count = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (b.board[i][j] != 0) {
                tile_count++;
                assert(b.board[i][j] == 2 || b.board[i][j] == 4);
            }
        }
    }
    assert(tile_count == 2);
    std::cout << "✓ test_board_initialization passed" << std::endl;
}

void test_spawn_tile() {
    Board b;
    int initial_tiles = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (b.board[i][j] != 0) initial_tiles++;
        }
    }
    
    b.spawnTile();
    
    int new_tiles = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (b.board[i][j] != 0) new_tiles++;
        }
    }
    
    assert(new_tiles == initial_tiles + 1);
    std::cout << "✓ test_spawn_tile passed" << std::endl;
}

void test_swipe_left() {
    Board b;
    // Set up a simple board state
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b.board[i][j] = 0;
        }
    }
    
    b.board[0][0] = 2;
    b.board[0][1] = 0;
    b.board[0][2] = 2;
    b.board[0][3] = 0;
    
    std::cout << "Before swipeLeft: [" << b.board[0][0] << ", " << b.board[0][1] << ", " << b.board[0][2] << ", " << b.board[0][3] << "]" << std::endl;
    
    b.swipeLeft();
    
    std::cout << "After swipeLeft: [" << b.board[0][0] << ", " << b.board[0][1] << ", " << b.board[0][2] << ", " << b.board[0][3] << "]" << std::endl;
    
    assert(b.board[0][0] == 4);
    assert(b.board[0][1] == 0);
    assert(b.board[0][2] == 0);
    assert(b.board[0][3] == 0);
    std::cout << "✓ test_swipe_left passed" << std::endl;
}

void test_swipe_right() {
    Board b;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b.board[i][j] = 0;
        }
    }
    
    b.board[0][0] = 2;
    b.board[0][1] = 0;
    b.board[0][2] = 2;
    b.board[0][3] = 0;
    
    std::cout << "Before swipeRight: [" << b.board[0][0] << ", " << b.board[0][1] << ", " << b.board[0][2] << ", " << b.board[0][3] << "]" << std::endl;
    
    b.swipeRight();
    
    std::cout << "After swipeRight: [" << b.board[0][0] << ", " << b.board[0][1] << ", " << b.board[0][2] << ", " << b.board[0][3] << "]" << std::endl;
    
    assert(b.board[0][3] == 4);
    assert(b.board[0][2] == 0);
    assert(b.board[0][1] == 0);
    assert(b.board[0][0] == 0);
    std::cout << "✓ test_swipe_right passed" << std::endl;
}

void test_is_game_over() {
    Board b;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b.board[i][j] = 0;
        }
    }
    
    // Fill board completely with no adjacent matches (alternating 2 and 4)
    // Ensure no horizontal or vertical adjacencies
    b.board[0][0] = 2;  b.board[0][1] = 4;  b.board[0][2] = 2;  b.board[0][3] = 4;
    b.board[1][0] = 4;  b.board[1][1] = 2;  b.board[1][2] = 4;  b.board[1][3] = 2;
    b.board[2][0] = 2;  b.board[2][1] = 4;  b.board[2][2] = 2;  b.board[2][3] = 4;
    b.board[3][0] = 4;  b.board[3][1] = 2;  b.board[3][2] = 4;  b.board[3][3] = 2;
    
    std::cout << "Board state for game over test:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  [" << b.board[i][0] << ", " << b.board[i][1] << ", " << b.board[i][2] << ", " << b.board[i][3] << "]" << std::endl;
    }
    
    assert(b.isGameOver() == true);
    std::cout << "✓ test_is_game_over passed" << std::endl;
}

void test_swipe_up() {
    Board b;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b.board[i][j] = 0;
        }
    }
    
    b.board[0][0] = 0;
    b.board[1][0] = 2;
    b.board[2][0] = 0;
    b.board[3][0] = 2;
    
    std::cout << "Before swipeUP (col 0): [" << b.board[0][0] << ", " << b.board[1][0] << ", " << b.board[2][0] << ", " << b.board[3][0] << "]" << std::endl;
    
    b.swipeUP();
    
    std::cout << "After swipeUP (col 0): [" << b.board[0][0] << ", " << b.board[1][0] << ", " << b.board[2][0] << ", " << b.board[3][0] << "]" << std::endl;
    
    assert(b.board[0][0] == 4);
    assert(b.board[1][0] == 0);
    assert(b.board[2][0] == 0);
    assert(b.board[3][0] == 0);
    std::cout << "✓ test_swipe_up passed" << std::endl;
}

void test_swipe_down() {
    Board b;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b.board[i][j] = 0;
        }
    }
    
    b.board[0][0] = 2;
    b.board[1][0] = 0;
    b.board[2][0] = 2;
    b.board[3][0] = 0;
    
    std::cout << "Before swipeDown (col 0): [" << b.board[0][0] << ", " << b.board[1][0] << ", " << b.board[2][0] << ", " << b.board[3][0] << "]" << std::endl;
    
    b.swipeDown();
    
    std::cout << "After swipeDown (col 0): [" << b.board[0][0] << ", " << b.board[1][0] << ", " << b.board[2][0] << ", " << b.board[3][0] << "]" << std::endl;
    
    assert(b.board[0][0] == 0);
    assert(b.board[1][0] == 0);
    assert(b.board[2][0] == 0);
    assert(b.board[3][0] == 4);
    std::cout << "✓ test_swipe_down passed" << std::endl;
}

int main() {
    test_board_initialization();
    test_spawn_tile();
    test_swipe_left();
    test_swipe_right();
    test_swipe_up();
    test_swipe_down();
    test_is_game_over();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
