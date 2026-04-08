#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

void get_input_time(int &start_time, int &end_time) {
    while (start_time < 0 || start_time > 2359) {
        cout << "Enter the start time in 24hr notation (HHMM): ";
        cin >> start_time;
    }
    while (end_time < 0 || end_time > 2359) {
        cout << "Enter the end time in 24hr notation (HHMM): ";
        cin >> end_time;
    }
}

// finds the amount of minutes between start and end times
int calculate_time(int start_time, int end_time) {
    int start_hour = start_time / 100;
    int start_minute = start_time % 100;

    int end_hour = end_time / 100;
    int end_minute = end_time % 100;

    int total_start_minutes = start_hour * 60 + start_minute;
    int total_end_minutes = end_hour * 60 + end_minute;

    if (total_end_minutes < total_start_minutes) {
        total_end_minutes += 24 * 60; // add 24 hours
    }

    return total_end_minutes - total_start_minutes;
}

int main(){
int start_time, end_time;
    get_input_time(start_time, end_time);
    int result = calculate_time(start_time, end_time);
    cout << "The amount of minutes between the start and end times is: " << result;
}