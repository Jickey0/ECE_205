/*
Define a class called BicycleSpeedometer that will track distance traveled by, travel time of, 
and the average speed of the rider. The class should have member variables for distance traveled (in 
miles), and the time taken (in minutes). Include mutator functions to set distance, and time. Include a 
member function to compute the average speed (in miles per hour). Use your class with a test program 
that creates the class, reads input values for all the required member variables and outputs the average 
speed of the rider.  
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

class BicycleSpeedometer {
    private:
        float distance; // in miles
        float time;     // in minutes
    public:
        void setDistance(float d){
            distance = d;
        }

        void setTime(float t){
            time = t;
        }

        float computeAverageSpeed(){
            return (distance * 60) / time; // return in miles per hour
        }
};

int main(){
    BicycleSpeedometer myBikeSpeedomter;

    float distance;
    float time;

    cout << "Welcome to Bicycle Speedometer 1.00" << endl;

    cout << "Input distance in miles" << endl;
    cin >> distance;
    myBikeSpeedomter.setDistance(distance);

    cout << "Input time in minutes" << endl;
    cin >> time;
    myBikeSpeedomter.setTime(time);

    cout << "Your average speed is: " << myBikeSpeedomter.computeAverageSpeed() << " mph" << endl;
}