#include <iostream>
#include <string>
using namespace std;

// Class definition
class Car {
private:
    string brand;   // Data member (private)
    int year;       // Data member (private)

public:
    // Constructor
    Car(string b, int y) {
        brand = b;
        year = y;
    }

    // Member function to display details
    void displayInfo() {
        cout << "Brand: " << brand << ", Year: " << year << endl;
    }

    // Setter function
    void setYear(int y) {
        if (y > 1885) { // First car invented around 1886
            year = y;
        } else {
            cout << "Invalid year!" << endl;
        }
    }

    // Getter function
    int getYear() {
        return year;
    }
};

int main() {
    // Create objects of Car
    Car car1("Toyota", 2020);
    Car car2("Ford", 2018);

    // Access member functions
    car1.displayInfo();
    car2.displayInfo();

    // Modify and access data using setters/getters
    car1.setYear(2022);
    cout << "Updated Year for Car1: " << car1.getYear() << endl;

    return 0;
}
