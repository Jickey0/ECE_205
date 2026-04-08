#include <iostream>
using namespace std;

int Health = 100;
bool isDead = false;

void TakeDamage(){
    cout << "taking damage\n";
    Health -= 10;
}

int main(){
    TakeDamage();

    if (Health <= 0) isDead = true;

    // example of flow control
    if (isDead) {
        DropLoot();
    } else {
        cout << "drop nothing\n";
    }

    // ternary operator example these two are equivalent
    isDead ? DropLoot() : cout << "drop nothing\n";


    // ternary assignment
    int Health{100};
    if (isDead) {
    Health = 0;
    }

    // same as 
    int Health { isDead ? 0 : 100 };

    // nested ternarys
    if (Health <= 0) {
        DropLoot();
    } else if (Health <= 50) {
        RunAway();
    } else {
        Attack();
    }

    Health <= 0
        ? DropLoot()
        : Health <= 50
        ? RunAway()
        : Attack();

    int Day{3};

    switch (Day) {
    case 1: // Is Day == 1?
        cout << "Monday";
        break;
    case 2: // Is Day == 2?
        cout << "Tuesday";
        break;
    case 3: // Is Day == 3?
        cout << "Wednesday";
        break;
    }

    // switching with default case
    // activated in all scenarios unless a previously -->
    // activated case triggered a break
    switch (Day) {
    case 1:
        cout << "Monday";
        break;
    case 2:
        cout << "Tuesday";
        break;
    default:
        cout << "Something else";
    }

    
}