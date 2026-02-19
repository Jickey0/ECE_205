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

int main(){

    do {
        int input_number;
        cout << "Enter a number: ";
        cin >> input_number;

        if (input_number > 0){
            cout << "The number is positive." << endl;
        } else if (input_number < 0){
            cout << "The number is negative." << endl;
        } else {
            cout << "The number is zero." << endl;
        }

    } while (ask_to_continue());
    
}