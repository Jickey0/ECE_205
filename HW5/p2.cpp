/*
Define a class called Pizza that has member variables to track the type of pizza (either deep 
dish, hand tossed, or pan) along with the size (either small, medium, or large) and the number of 
pepperoni or cheese toppings. You can use constants to represent the type and size. Include mutator 
and accessor functions for your class. Create a void function, outputDescription( ), that outputs a 
textual description of the pizza object. Also include a function, computePrice( ), that computes the cost 
of the pizza and returns it as a double according to the following rules:  

Small pizza = $10 + $2 per topping 
Medium pizza = $14 + $2 per topping 
Large pizza = $17 + $2 per topping 

Write a suitable test program that creates and outputs a description and price of various pizza objects. 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

class Pizza {
    private: 
        int type;      // 0 = deep dish, 1 = hand tossed, 2 = pan 
        int size;      // 0 = small, 1 = medium, 2 = large

        int pepperoni; // number of pepperoni toppings
        int cheese;    // number of cheese toppings
    public: 
        // display information about the pizza 
        void outputDescription(){
            if (type == 0){
                cout << "The pizza is deep dish ";
            }
            if (type == 1){
                cout << "The pizza is hand tossed ";
            }
            if (type == 2){
                cout << "The pizza is pan ";
            }

            if (size == 0){
                cout << "and it is a small sized. " << endl;
            }
            if (size == 1){
                cout << "and it is a medium sized. " << endl;
            }
            if (size == 2){
                cout << "and it is a large sized. " << endl;
            }

            cout << "It has " << pepperoni << " pepperoni toppings and " << cheese << " cheese toppings." << endl;
            cout << "The total cost is: " << computePrice() << "$" << endl;
        }

        // computes the cost of the pizza and returns it as a double
        double computePrice(){
            if (size == 0){
                return (10 + 2*pepperoni + 2*cheese);
            }
            if (size == 1){
                return (14 + 2*pepperoni + 2*cheese);
            }
            if (size == 2){
                return (17 + 2*pepperoni + 2*cheese);
            }
        }

        void setPizzaParameters(int type_input, int size_input, int pepperoni_input, int cheese_input){
            type = type_input;
            size = size_input;
            pepperoni = pepperoni_input;
            cheese = cheese_input;
        }

        // not used, but defined for more user customization
        int getType() {
            return type;
        }

        int getSize() {
            return size;
        }

        int getPepperoni() {
            return pepperoni;
        }

        int getCheese() {
            return cheese;
        }
};

int main(){
    for (int i = 0; i < 3; i++){
        cout << "Generating Pizza number " << i+1 <<endl;

        // define pizza object, set private values, display results
        Pizza currentPizza;
        currentPizza.setPizzaParameters(i, i, i*2, i*3);
        currentPizza.outputDescription();
    }
};