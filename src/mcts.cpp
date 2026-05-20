#include "mcts.h"
#include <vector>
#include "board.h"
#include <limits>
#include <cmath>
using namespace std;

MCTS::MCTS(int simulation) : simulation_(simulation) {

}

double MCTS::ucb1(decisionNode* root){
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

int MCTS::getBestMove(Board& board){

}