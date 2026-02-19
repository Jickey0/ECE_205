#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

// Function to display the output time in 12hr notation
void display_output(int hour, int minute, char AM_PM){
    cout << "The time in 12hr notation is: " << hour << ":" << minute << " " << AM_PM << "M" << endl;
}

// Function to get input time from user and validate it
void get_input_time(int &hour, int &minute) {
    while (hour < 0 || hour > 23) {
        cout << "Enter the hour: ";
        cin >> hour;
    }

    while (minute < 0 || minute > 59) {
        cout << "Enter the minute: ";
        cin >> minute;
    }
}

// Function to convert 24hr time to 12hr time and determine AM/PM
int convert_to_12hr(int hour, char &AM_PM){
    if (hour > 12){
        hour -= 12;
        AM_PM = 'P';
    } else {
        AM_PM = 'A';
    }
    return hour;
}

// Function to ask user if they want to continue
int ask_user_to_continue(){
    char user_input;
    cout << "Do you want to continue? (Y/N): ";
    cin >> user_input;
    if (user_input == 'Y' || user_input == 'y') {
        return 1;
    } else {
        return 0;
    }
}

// Main function to run the program
int main(){
    int hour = 100;
    int minute = 100;
    char AM_PM;

    do {
        hour = 100;
        minute = 100;
        get_input_time(hour, minute);
        hour = convert_to_12hr(hour, AM_PM);
        display_output(hour, minute, AM_PM);
    } while ((ask_user_to_continue()));
}
