#include <iostream>
#include <string>
using namespace std;

class Robot {
public:
    string name;

    // Defined INSIDE the class
    Robot(string n) {
        name = n;
        cout << name << " is powered up!" << endl;
    }
};

int main() {
    Robot myBot("R2-D2"); // Constructor called here
    return 0;
}