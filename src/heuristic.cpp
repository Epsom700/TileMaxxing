#include "heuristic.h"
#include <cmath>
#include <cstdlib>
using namespace std; 

double scoreBoard(const int board[4][4]){
    int zero_count = 0; 
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            if (board[i][j] == 0) ++zero_count;
        }
    }
    double accumulator = 0.0; 
    for (int i = 0; i < 4; ++i){
        for (int j =0; j < 4; ++j){
            if (board[i][j]!=0){
                if ((j < 3)&&(board[i][j+1]!=0)){
                    accumulator+=fabs(log2(board[i][j]) - log2(board[i][j+1])); 
                }
                if ((i < 3)&&(board[i+1][j]!=0)){
                    accumulator+=fabs(log2(board[i][j])-log2(board[i+1][j]));
                }
            }
        }
    }
    double output = (2.7*zero_count) - (0.1*accumulator) + 0.1; 
    return output;
}