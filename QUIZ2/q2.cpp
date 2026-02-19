#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

bool ask_to_continue() {
    char user_input;
    cout << "Do you want to continue? (Y/N): ";
    cin >> user_input;
    if (user_input == 'Y' || user_input == 'y') {
        return true;
    } else {
        return false;
    }
}

bool inOrder(){
    int first_number, second_number, third_number;
    cout << "Enter three numbers: ";
    cin >> first_number >> second_number >> third_number;

    if (first_number < second_number && second_number < third_number){
        return true;
    } else {
        return false;
    }
}

void main(){
    do {
        if (inOrder()){
            cout << "The numbers are in order." << endl;
        } else {
            cout << "The numbers are not in order." << endl;
        }
    } while (ask_to_continue());
}