#include "mcts.h"
#include <vector>
#include "board.h"
#include <limits>
#include <cmath>
#include <random>
using namespace std;

MCTS::MCTS(int simulation) : simulation_(simulation), gen_(random_device{}()) {

}

double MCTS::ucb1(chanceNode* root){
    // UCB1 = (total_score / visit_count) + C * sqrt(ln(parent_visits) / visit_count)
    if (root->visit_counter==0)return numeric_limits<double>::infinity(); 
    if (root->parent==nullptr) return numeric_limits<double>::infinity(); 
    int vi = root->visit_counter; 
    double total_score = root->total_score; 
    int parent_visit = root->parent->visit_counter; 
    double c = sqrt(2); 

    double value = (total_score/vi) + c * sqrt(log(parent_visit)/vi); 
    return value; 

}
pair<MCTS::decisionNode*, MCTS::chanceNode*> MCTS::select(decisionNode* root){
    decisionNode* track = root; 
    while ((!track->child.empty())&&(track->is_terminal==false)){
        double best_value = -numeric_limits<double>::infinity();
        chanceNode* best_node = nullptr; 
        for (auto* c : track->child){
            double temp = ucb1(c); 
            if (best_value < temp){
                best_value = temp; 
                best_node = c; 
            }
        }

        vector<pair<int, int>> zero_track; 
        auto& b = best_node->board;
        for (int i = 0; i< 4; ++i){
            for (int j=0; j<4;++j){
                if (b[i][j]==0) zero_track.push_back({i, j}); 
            }
        }
 
        int len_track_loop = zero_track.size(); 
        discrete_distribution<int> distr_2({90, 10}); 
        uniform_int_distribution<int> distr(0, len_track_loop-1);
        int index = distr(gen_);
        pair<int, int> cell =zero_track[index]; 
        int tile = (distr_2(gen_) == 0) ? 2 : 4; 
        pair<int, pair<int, int>> tile_val_pos = {tile, cell};
        auto it = best_node->children.find(tile_val_pos); 
        if (it != best_node->children.end()){
            track = it->second; 
        }else{
            last_sampled_key_ = tile_val_pos;
            return {track, best_node}; 
        }
    }
    return {track, nullptr};

}


MCTS::decisionNode* MCTS::createDecisionNode(chanceNode* parent, pair<int, pair<int, int>> tile_val_pos){
    decisionNode* new_node = new decisionNode; 
    new_node->parent = parent; 
    memcpy(new_node->board, parent->board, sizeof(parent->board));
    pair<int, int> tile_pos_ = tile_val_pos.second; 
    new_node->board[tile_pos_.first][tile_pos_.second] = tile_val_pos.first; 
    new_node->tile_pos = tile_pos_; 
    new_node->tile_val = tile_val_pos.first; 
    new_node->visit_counter = 0; 
    new_node->total_score = 0; 
    Board temp;
    memcpy(temp.board, new_node->board, sizeof(temp.board));
    new_node->is_terminal = temp.isGameOver();
    parent->children[tile_val_pos] = new_node; 
    return new_node; 
}

MCTS::chanceNode* MCTS::createChanceNode(decisionNode* parent, int move, const int board_after_move[4][4]){
    chanceNode* new_node = new chanceNode; 
    memcpy(new_node->board, board_after_move, sizeof(new_node->board)); 
    new_node->move = move; 
    new_node->visit_counter = 0; 
    new_node->total_score = 0; 
    new_node->parent = parent; 
    parent->child.push_back(new_node); 
    return new_node; 

}

MCTS::decisionNode* MCTS::expand(pair<decisionNode*, chanceNode*> frontier){

}


int MCTS::getBestMove(Board& board){
    
}