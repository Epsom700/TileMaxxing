#include "board.h"
#include <iostream>
#include <random>
#include <cmath>
using namespace std;

Board::Board() : gen(random_device{}()) {
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j< 4; ++j){
            board[i][j] = 0; 
        }
    }
    score = 0; 
    gameOver = false;
    spawnTile();
    spawnTile();
}

void Board::spawnTile(){

    discrete_distribution<int> distr_2({90, 10}); 
    vector<pair<int, int>> track_loop; 
    for (int i = 0; i< 4; ++i){
        for (int j = 0; j< 4; ++j){
            if (board[i][j]==0){ 
                track_loop.push_back({i, j}); 
            }
        }
    }
    if (track_loop.empty()){
        gameOver = isGameOver(); 
        return; 
    } 
    int row, col; 
    int len_track_loop = track_loop.size(); 
    uniform_int_distribution<int> distr(0, len_track_loop-1);
    int index = distr(gen); 
    row = track_loop[index].first; 
    col = track_loop[index].second; 
    int tile = (distr_2(gen) == 0) ? 2 : 4; 
    board[row][col] = tile; 

}

void Board::swipeRow(int rows[4], int final_row[4]){
    memset(final_row, 0, 4 * sizeof(int));
    bool merged = false; 
    int write_index = 0; 

    for (int i0=0; i0< 4 ; ++i0){
        int i = rows[i0]; 
        if (i==0) continue;
        if ((write_index > 0) && (final_row[write_index-1] == i) && (merged == false)){
            final_row[write_index-1] += i; 
            score +=2*i; 
            merged=true; 
        }else{
            final_row[write_index] = i; 
            merged = false; 
            ++write_index; 
        }

    }

}


void Board::swipeLeft(){
    for (int i =0;i < 4; ++i){
        int temp_array[4]; 
        for (int j =0; j< 4; ++j){
            temp_array[j] = board[i][j];   
        }
        int result[] = {0,0,0,0};
        swipeRow(temp_array, result); 
        for (int k = 0; k< 4; ++k) board[i][k] = result[k]; 
    }
}

void Board::swipeRight(){
    for (int i =0;i < 4; ++i){
        int temp_array[4]; 
        for (int j =3; j>=0; --j){
            temp_array[3-j] = board[i][j]; 
        }
        int result[] = {0,0,0,0};
        swipeRow(temp_array, result); 
        for (int k = 3; k>=0; --k) board[i][k] = result[3-k]; 
    }
}

void Board::swipeUP(){
    for (int i =0;i < 4; ++i){
        int temp_array[4]; 
        for (int j =0; j<4; ++j){
            temp_array[j] = board[j][i];   
        }
        int result[] = {0,0,0,0};
        swipeRow(temp_array, result); 
        for (int k = 0; k<4; ++k) board[k][i] = result[k]; 
    }
}

void Board::swipeDown(){
    for (int i =3;i >= 0; --i){
        int temp_array[4]; 
        for (int j =3; j>=0; --j){
            temp_array[3-j] = board[j][i];   
        }
        int result[] = {0,0,0,0};
        swipeRow(temp_array, result); 
        for (int k = 0; k < 4; ++k) board[3-k][i] = result[k];
    }
}

bool Board::isGameOver(){
    for (int i=0; i< 4; i++){
        for (int j =0; j< 4; j++){
            if (board[i][j]==0) return false; 
            if ((j < 3) && board[i][j]==board[i][j+1]) return false; 
            if ((i < 3)&& board[i][j] == board[i+1][j]) return false; 
        }
    }
    return true; 
}
