#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

void take_input_values(float &lbs, float &oz) {
    // code to take input values from user
    cout << "Enter the weight in pounds: ";
    cin >> lbs;
    cout << "Enter the weight in ounces: ";
    cin >> oz;
}

void calculate_results(float lbs, float oz, float &kg, float &g) {
    // code to calculate the results
    kg = lbs * 0.453592 + oz * 0.0283495;
    g = kg * 1000;
}

void display_results(float kg, float g) {
    // code to display the results
    if (kg >= 1){
        cout << "The weight is " << kg << " kg and " << g << " g." << endl;
    } else {
        cout << "The weight is " << g << " g." << endl;
    }
}

int main(){
    float lbs, oz;
    float kg, g;

    take_input_values(lbs, oz);
    calculate_results(lbs, oz, kg, g);
    display_results(kg, g);
}