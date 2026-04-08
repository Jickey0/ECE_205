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
            time = t
        }

        int computeAverageSpeed(){
            return distance / time
        }
}

int main(){
    
}