#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

void calculate_area(float a, float b, float c, float &s, float &area) {
    // calculate the semi-perimeter
    s = (a + b + c) / 2;

    // calculate the area using the formula: Area = sqrt(s * (s - a) * (s - b) * (s - c))
    area = sqrt(s * (s - a) * (s - b) * (s - c));
}

void main(){
    // define variables
    float a, b, c;
    float s, area;

    while (true) {
        // get user input
        cout << "Enter the lengths of the three sides of the triangle: ";
        cin >> a >> b >> c;

        // check if the sides can form a triangle
        if (a + b > c && a + c > b && b + c > a) {
            break;
        } else {
            cout << "The lengths entered cannot form a triangle. Please try again." << endl;
        }
    }

    // calculate the area using the formula: Area = sqrt(s * (s - a) * (s - b) * (s - c))
    calculate_area(a, b, c, s, area);

    // print the area
    cout << "The area of the triangle is: " << area << endl;
}