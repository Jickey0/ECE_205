#include <iostream>
using namespace std;

class Drone {
private:
    int speed = 0; // Private data
    static int droneCount; // Static data

public:
    Drone() { droneCount++; }

    // STATIC FUNCTION: Belongs to the class
    static int getCount() {
        return droneCount; 
    }

    // MUTATOR: Changes the speed (with a safety check)
    void setSpeed(int s) {
        if (s >= 0) speed = s;
    }

    // ACCESSOR: Simply returns the speed
    int getSpeed() {
        return speed;
    }
};

// Initialize static member
int Drone::droneCount = 0;

int main() {
    Drone d1, d2;

    d1.setSpeed(50); // Using a Mutator
    cout << "Drone Speed: " << d1.getSpeed() << endl; // Using an Accessor

    // Calling static function via the Class Name
    cout << "Total Drones: " << Drone::getCount() << endl; 

    return 0;
}