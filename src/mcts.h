#include <vector>
#include "board.h"
class MCTS{
    public: 
        MCTS(int simulation_); 
        int getBestMove(Board& board); 
        struct chanceNode; 
        struct decisionNode
        {
            int board[4][4]; 
            int move;
            int visit_counter; 
            double total_score; 
            decisionNode* parent; 
            std::vector<chanceNode*> child; 
        };
        struct chanceNode
        {
            int board[4][4]; 
            int tile_val; 
            std::pair<int,int> tile_position; 
            decisionNode* parent; 
            decisionNode* child; 
            int visit_counter; 
            double total_score; 
        };

    private: 
        int simulation_; 
        void deleteTrees(decisionNode* root);         
        double ucb1(decisionNode* root); 
};