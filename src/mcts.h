#include <vector>
#include <map>
#include <utility>
#include "board.h"
class MCTS{
    public: 
        MCTS(int simulation_);
        enum Directions {left = 0, right, up, down}; 
        int getBestMove(Board& board); 
        struct chanceNode; 
        struct decisionNode
        {
            int board[4][4]; 
            int visit_counter; 
            int tile_val; 
            std::pair<int, int> tile_pos; 
            double total_score; 
            chanceNode* parent; 
            bool is_terminal;
            std::vector<chanceNode*> child; 
        };
        struct chanceNode
        {
            int board[4][4]; 
            int move; 
            decisionNode* parent; 
            std::map<std::pair<int,std::pair<int,int>>, decisionNode*> children;
            int visit_counter; 
            double total_score; 
        };

    private: 
        int simulation_; 
        void deleteTrees(decisionNode* root);  
        std::pair<decisionNode*, chanceNode*> select(decisionNode* root);  
        decisionNode* expand(std::pair<decisionNode*, chanceNode*> node); 
        decisionNode* createDecisionNode(chanceNode* parent, std::pair<int, std::pair<int, int>> tile_val_pos);
        chanceNode* createChanceNode(decisionNode* parent, int move, const int board_after_move[4][4]);  
        double ucb1(chanceNode* root); 
        std::mt19937 gen_;
        std::pair<int, std::pair<int, int>> last_sampled_key_; 
};