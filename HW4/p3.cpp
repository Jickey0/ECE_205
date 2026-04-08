/*
Write a program to assign passengers seats in an airplane. Assume a single-aisle airplane with seat 
numbers as follows (10 rows of 4 seats each): 

A B  C D 
A B  C D 
A B  C D 
A B  C D 

The program should display the seat pattern as shown below, where ‘X’ marks the taken seats (in the example 
below, 1B, 3D and 5A are taken). 

A X  C D 
A B  C D 
A B  C X 
A B  C D 
X B  C D 
A B  C D 
A B  C D 
A B  C D 
A B  C D 
A B  C D 

After displaying the seats available, the program will prompt the user for the desired seat (taken as 2 inputs – one 
integer and one char) and then update the seat chart if the requested seat is available. If the seat is not available, 
the program will prompt the user for a seat until a valid entry is made. Once the seat pattern is updated, the 
program will ask the user if they want to continue adding seats until all seats are taken or if the user does not want 
to take any other seats. 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector>

using namespace std;

// prototypes
void askForSeat();
bool isSeatAvailable(int row, char seat);
bool areSeatsAvailable();
void assignSeat(int row, char seat);
void displayAvailableSeats();

// initialize global seat arrays (0 for available, 1 for taken)
int rowA[10] = {0};
int rowB[10] = {0};
int rowC[10] = {0};
int rowD[10] = {0};

int main(){
    while (areSeatsAvailable()) {
        displayAvailableSeats();
        askForSeat();

        char continueChoice;
        cout << "Do you want to continue adding seats? (Y/N): ";
        cin >> continueChoice;
        if (continueChoice == 'N' || continueChoice == 'n') {
            break;
        }
    }

    return 0;
}

void askForSeat(){
    int row;
    char seat;
    cout << "Enter desired seat: ";
    cin >> row >> seat;

    if (isSeatAvailable(row, seat)) {
        assignSeat(row, seat);
    } else {
        cout << "Seat is not available. Please choose another seat." << endl;
        askForSeat();
    }
}

bool isSeatAvailable(int row, char seat){
    if (seat == 'A') {
        return rowA[row-1] == 0;
    } else if (seat == 'B') {
        return rowB[row-1] == 0;
    } else if (seat == 'C') {
        return rowC[row-1] == 0;
    } else if (seat == 'D') {
        return rowD[row-1] == 0;
    }
    return false; // Invalid seat
}

bool areSeatsAvailable(){
    for (int i = 0; i < 10; i++) {
        if (rowA[i] == 0 || rowB[i] == 0 || rowC[i] == 0 || rowD[i] == 0) {
            return true;
        }
    }
    return false;
}

void assignSeat(int row, char seat){
    if (seat == 'A') {
        rowA[row-1] = 1;
    } else if (seat == 'B') {
        rowB[row-1] = 1;
    } else if (seat == 'C') {
        rowC[row-1] = 1;
    } else if (seat == 'D') {
        rowD[row-1] = 1;
    }
}

void displayAvailableSeats(){
    cout << "Available seats: " << endl;
    for (int i = 0; i < 10; i++) {
        cout << "Row " << i+1 << ": ";
        cout << (rowA[i] == 0 ? "A " : "X ");
        cout << (rowB[i] == 0 ? "B " : "X ");
        cout << (rowC[i] == 0 ? "C " : "X ");
        cout << (rowD[i] == 0 ? "D" : "X") << endl;
    }
}

