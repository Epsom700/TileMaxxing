#include "mcts.h"
#include "board.h"
#include "heuristic.h"
#include <vector>
#include <limits>
#include <cmath>
#include <random>
#include <cassert>
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
    double c = 1000.0;

    double value = (total_score/vi) + c * sqrt(log(parent_visit)/vi); 
    return value; 

}


pair<int, pair<int, int>> MCTS::sampleTile(const int board[4][4]){
    vector<pair<int, int>> zero_track; 
    for (int i = 0; i < 4; ++i){
        for (int j=0; j<4; ++j){
            if (board[i][j] == 0) {
                zero_track.push_back({i, j});
            }
        }
    }
    assert(!zero_track.empty()); // Return a default tile value and position if no empty cells are found
    int len_track_loop = zero_track.size(); 
    discrete_distribution<int> distr_2({90, 10}); 
    uniform_int_distribution<int> distr(0, len_track_loop-1);
    int index = distr(gen_);
    pair<int, int> cell =zero_track[index]; 
    int tile = (distr_2(gen_) == 0) ? 2 : 4; 
    pair<int, pair<int, int>> tile_val_pos = {tile, cell};
    return tile_val_pos;
    
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
        auto tile_val_pos = sampleTile(best_node->board);
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
    if (frontier.second != nullptr){
        return createDecisionNode(frontier.second, last_sampled_key_); 
    }else if (frontier.first->is_terminal && frontier.second == nullptr){
        return frontier.first; 
    }
    vector<chanceNode*> legal_moves;
    for (int move_ = 0; move_ < 4; ++move_){
        Board temp; 
        memcpy(temp.board, frontier.first->board, sizeof(temp.board));
        switch (move_){
            case 0: temp.swipeLeft(); break;
            case 1: temp.swipeRight(); break;
            case 2: temp.swipeUP(); break;
            case 3: temp.swipeDown(); break;    
        }
        if (memcmp(temp.board, frontier.first->board, sizeof(temp.board))!=0){
            legal_moves.push_back(createChanceNode(frontier.first, move_, temp.board));
        }
    }
    if (legal_moves.empty()) {
        frontier.first->is_terminal = true;   // mark it so future select() catches this
        return frontier.first;
    }
    uniform_int_distribution<int> distr(0, legal_moves.size() - 1);
    chanceNode* selected_move = legal_moves[distr(gen_)];
    auto tile_val_pos = sampleTile(selected_move->board);
    return createDecisionNode(selected_move, tile_val_pos);
}

bool MCTS::applyRandomMove(Board& b){
    vector<int> legal;
    for (int move = 0; move < 4; ++move){
        Board temp;
        memcpy(temp.board, b.board, sizeof(temp.board));
        temp.score = 0;

        switch (move){
            case 0: temp.swipeLeft(); break;
            case 1: temp.swipeRight(); break;
            case 2: temp.swipeUP(); break;
            case 3: temp.swipeDown(); break;
        }

        bool changed = false;
        for (int i = 0; i < 4 && !changed; ++i){
            for (int j = 0; j < 4 && !changed; ++j){
                if (temp.board[i][j] != b.board[i][j]) changed = true;
            }
        }

        if (changed) legal.push_back(move);
    }

    if (legal.empty()) return false;

    uniform_int_distribution<int> distr(0, (int)legal.size() - 1);
    int pick = legal[distr(gen_)];

    switch (pick){
        case 0: b.swipeLeft(); break;
        case 1: b.swipeRight(); break;
        case 2: b.swipeUP(); break;
        case 3: b.swipeDown(); break;
    }

    return true;
}


bool MCTS::applyHeuristicMove(Board& b){
    double best_score = -numeric_limits<double>::infinity(); 
    vector<int> best_move;
    for (int move_=0; move_ < 4; ++move_){
        Board temp;
        memcpy(temp.board, b.board, sizeof(temp.board));
        temp.score = 0;
        switch (move_){
            case 0: temp.swipeLeft(); break;
            case 1: temp.swipeRight(); break; 
            case 2: temp.swipeUP(); break; 
            case 3: temp.swipeDown(); break; 
        }

        if (memcmp(temp.board, b.board, sizeof(temp.board))){
            double curr_score = scoreBoard(temp.board); 
            if (curr_score > best_score){
                best_score = curr_score; 
                best_move.clear(); 
                best_move.push_back(move_);
            } else if (curr_score == best_score){
                best_move.push_back(move_); 
            }
        }
    }
    if (best_move.empty()) return false; 
    uniform_int_distribution<int> distr(0, (int)best_move.size() - 1);
    int pick = best_move[distr(gen_)];
    switch (pick){
        case 0: b.swipeLeft(); break;
        case 1: b.swipeRight(); break;
        case 2: b.swipeUP(); break;
        case 3: b.swipeDown(); break;
    }
    return true;  
}

double MCTS::rollout(decisionNode* start){
    if (start->is_terminal) return 0.0;

    Board temp;
    memcpy(temp.board, start->board, sizeof(temp.board));
    temp.score = 0;

    while (true){
        if (!applyHeuristicMove(temp)) break;
        temp.spawnTile();
    }

    return (double)temp.score;
}


void MCTS::backprop(decisionNode* dn, double rollout_value) {
    while (dn != nullptr) {
        dn->visit_counter++;
        dn->total_score += rollout_value;
        chanceNode* cn = dn->parent;
        if (cn == nullptr) return;
        cn->visit_counter++;
        cn->total_score += rollout_value;
        dn = cn->parent;
    }
}
void MCTS::deleteTrees(decisionNode* root){
    if (root == nullptr) return;
    for (auto* c : root->child){
        for (auto& kv : c->children){
            deleteTrees(kv.second);
        }
        delete c;
    }
    delete root;
}
MCTS::MoveResult MCTS::getBestMove(Board& board){
    decisionNode* root = new decisionNode;
    memcpy(root->board, board.board, sizeof(root->board));
    root->parent = nullptr;
    root->tile_val = -1;
    root->tile_pos = {-1, -1};
    root->visit_counter = 0;
    root->total_score = 0;

    Board temp;
    memcpy(temp.board, board.board, sizeof(temp.board));
    root->is_terminal = temp.isGameOver();
    assert(!root->is_terminal);
    for (int i = 0; i < simulation_; ++i) {
        auto frontier = select(root);
        decisionNode* leaf = expand(frontier);
        double v = rollout(leaf);
        backprop(leaf, v);
    }
    int best_move = -1;
    int best_visits = -1;
    for (auto* c : root->child){
        if (c->visit_counter > best_visits){
            best_visits = c->visit_counter;
            best_move = c->move;
        }
    }
    assert(best_move != -1);
    assert(root->visit_counter > 0);
    double eval = root->total_score / root->visit_counter;
    deleteTrees(root);
    return {best_move, eval};
}