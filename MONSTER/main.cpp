#include <iostream>
using namespace std;

int main() {
    // hell yeah
    bool isAlive;

    isAlive = true;

    // initialization of a variable
    bool isDead = {false};

    cout << "The monster is alive: " << isAlive << "\n";

    float Armor = 99.5f;

    cout << "The monster's armor is: " << Armor << "\n";

    Armor *= 3.0f;

    cout << "The monster's armor after upgrade is: " << Armor << "\n";

    // dividing ints rounds down --> truncation
    cout << "7 / 4 = " << 7 / 4;

    float Health { 100.0 };

    // True example
    bool isHealthy = Health > 50.0;

    // flow control, can use >=, <, <=, ==, !=
    if (Health){
        cout << "\nThe monster is healthy!\n";
    }
    else {
        cout << "\nThe monster is not healthy!\n";
    }

    // This will be true
    bool isReady { Health == 10.0f && isAlive };    
    cout << "Is the monster ready? " << isReady << "\n";



    return 0;
}