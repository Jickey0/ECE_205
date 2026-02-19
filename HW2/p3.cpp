#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

int main() {
    // find user's height, weight, age
    float height, weight, age;

    cout << "Enter your height in inches: ";
    cin >> height;
    cout << "Enter your weight in pounds: ";
    cin >> weight;
    cout << "Enter your age in years: ";
    cin >> age;

    // find hat size
    float hat_size = 2.9 * (weight / height);

    // find jacket size
    float jacket_size = (height * weight) / 288;

    if (age > 40) {
        float high_to_add = floorf((age - 30) / 10);
        jacket_size += high_to_add / 8;
    }

    // find waist size
    float waist_size = weight / 5.7;

    if (age > 28) {
        float high_to_add = floorf((age - 28) / 2);
        waist_size += high_to_add / 10;
    }

    // print results
    cout << "YOUR RESULTS ARE..." << endl;

    cout << "Hat Size: " << hat_size << endl;
    cout << "Jacket Size: " << jacket_size << endl;
    cout << "Waist Size: " << waist_size << endl;

    cout << "Thank you for using the Clothing Size Calculator!" << endl;
}