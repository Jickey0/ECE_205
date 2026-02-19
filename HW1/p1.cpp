/* Write a program that will take as input from the console an integer that represents a length 
of time in seconds. The program will then output the number of hours, minutes, and seconds that 
corresponds to the user input. In addition, the program will output the number of hours as a real 
number and the number of minutes as a real number. For example, if the user enters 50391 seconds, 
then the program will first output 13 hours, 59 minutes, and 51 seconds. Then the program will output 
13.9975 hours, and then output 839.8500 minutes. Use appropriate variable/constant names and types 
and format your output neatly. Format your real numbers to have 4 digits past the decimal point/radix. 
Test your code with several inputs to make sure the calculations are correct. */

// Jack Hickey

#include <iostream>
#include <cmath> // used for rounding functions
using namespace std;

int main(){
    cout << "Please provide a time in seconds: ";
    float time;
    cin >> time; // secs

    // find the largest value of hours that fits into the alloted time --> auto truncated from float to int
    int hours = time / 3600;
    int minutes = (int(time / 60)) % 60; // use modulo to get remaining minutes
    int seconds = time - (hours * 3600) - (minutes * 60);

    cout << "The time is: " << hours << " hours, " << minutes << " minutes, and " << seconds << " seconds\n";

    // Now we can show the exact hours and minutes as floats
    float exact_hours = time / 3600;
    float exact_minutes = time / 60;

    cout << "The exact time in hours is: " << floorf(exact_hours * 10000) / 10000 << " hours\n";
    cout << "The exact time in minutes is: " << floorf(exact_minutes * 10000) / 10000 << " minutes\n";    
}

