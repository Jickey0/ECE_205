#include <iostream>
#include <cmath>
using namespace std;

int main() {
    float G = 6.67430e-11; // gravitational constant in m^3 kg^-1 s^-2
    float F, m1, m2, r;

    cout << "Enter mass 1 (kg) \n";
    cin >> m1;
    cout << "mass 2 (kg) \n";
    cin >> m2;
    cout << "and distance (m): \n";
    cin >> r;
    F = G * (m1 * m2) / (r * r);
    cout << "The gravitational force is: " << F << " N" << endl;
    return 0;
}