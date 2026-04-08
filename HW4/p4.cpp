/*
 Write a program to play a game of tic-tac-toe. The users will alternate between entering an ‘X’ and an 
‘O’ by entering the location in the 3×3 matrix as shown below. 
1 2 3 
4 5 6 
7 8 9 
Player X starts the game by entering a position number, e.g. 3. Check whether this position is available and in 
range and output the current game status. 
1 2 X 
4 5 6 
7 8 9 
Once a valid position is entered and the current status is displayed, check whether the current player has won the 
game. If not, continue playing with alternate users until there is a winner or no more moves are possible. A win is 
defined as having the same character (‘X’ or ‘O’) on all positions of a row, column, or diagonal.
*/

#include <iostream>
#include <vector>

using namespace std;

char board[9] = {30, 30, 30, 30, 30, 30, 30, 30, 30}; // 0 for empty, 1 for X, 2 for O
char currentPlayer = 'X'; // Start with player X
bool draw = false;

// prototypes
void displayBoard();
void playMove();
bool gameOver();

int main(){
    cout << "TIC-TAC-TOE: Play a move by typing in the position number " << endl;

    while(gameOver() == 0){
        displayBoard();
        playMove();
    }

    if (draw == false){
        cout << "Player " << currentPlayer << " wins!" << endl;
    } else {
        cout << "It's a draw!" << endl;
    }
    return 0;
}

void displayBoard(){
    cout << "It is player " << currentPlayer << "'s turn" << endl;
    cout << " " << board[0] << " " << board[1] << " " << board[2] << endl;
    cout << " " << board[3] << " " << board[4] << " " << board[5] << endl;
    cout << " " << board[6] << " " << board[7] << " " << board[8] << endl;
}

void playMove(){
    int move;
    cin >> move;

    if (board[move - 1] == 0){
        board[move - 1] = currentPlayer;

        if (currentPlayer == 'X'){
            currentPlayer = 'O';
        } else {
            currentPlayer = 'X';
        }
    } else {
        cout << "Please enter a valid move " << endl;
        cin >> move;
    }
}

bool gameOver(){
    // Check rows
    for (int i = 0; i < 9; i += 3) {
        if (board[i] != 30 && board[i] == board[i+1] && board[i] == board[i+2]) {
            return true;
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] != 30 && board[i] == board[i+3] && board[i] == board[i+6]) {
            return true;
        }
    }

    // Check diagonals
    if (board[0] != 30 && board[0] == board[4] && board[0] == board[8]) {
        return true;
    }
    if (board[2] != 30 && board[2] == board[4] && board[2] == board[6]) {
        return true;
    }

    // Check for a draw
    for (int i = 0; i < 9; i++) {
        if (board[i] == 30) {
            return false;
        }
    }
    draw = true;

    return true;
}