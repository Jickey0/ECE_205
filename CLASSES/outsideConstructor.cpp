#include <iostream>
#include <string>
using namespace std;

class Pilot {
public:
    string name;
    Pilot(string n); // Declaration only
};

// Defined OUTSIDE the class
Pilot::Pilot(string n) {
    name = n;
    cout << "Pilot " << name << " is ready for takeoff." << endl;
}

int main() {
    Pilot myPilot("Skywalker"); 
    return 0;
}