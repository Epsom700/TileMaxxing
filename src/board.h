#include <random>

class Board {
    public: 
        Board();
        int board[4][4]; 
        void swipeUP(); 
        void swipeDown(); 
        void swipeLeft(); 
        void swipeRight();
        void spawnTile(); 
        bool isGameOver(); 
        int score;  
        bool gameOver; 

    private: 
        void swipe(); 
        void swipeRow(int row[4], int final_row[4]); 
        std::mt19937 gen;
        
        
};