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
            return 0;
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
        
        double getPrice() {
            return computePrice();
        }          
};

// Order class that contains a private vector of type Pizza.
// This class represents a customer’s entire order, where the order may consist of multiple pizzas.
class Order {
    private: 
        int orderIndex = 0;
        vector<Pizza> PizzaOrder; 

    public:
        // start order
        void begin(){
            cout << "Please order one of our world famous pizzas! " << endl;
            takeOrder();
            while (userWishesToContinue()){
                takeOrder();
            }
        }

        // check if the user wishes to continue their order
        bool userWishesToContinue(){
            char response;
            cout << "Do you wish to order another pizza? [Y/N]: " << endl;
            cin >> response;

            if (response == 'Y' || response == 'y'){
                return 1;
            }
            return 0;
        }

        // ask for each parameter in the order
        void takeOrder(){
            Pizza pizza;

            int type;      // 0 = deep dish, 1 = hand tossed, 2 = pan 
            int size;      // 0 = small, 1 = medium, 2 = large
            int pepperoni; // number of pepperoni toppings
            int cheese;    // number of cheese toppings

            cout << "Please enter pizza type (0 = deep dish, 1 = hand tossed, 2 = pan): " << endl;
            cin >> type;

            cout << "Please enter pizza size (0 = small, 1 = medium, 2 = large): " << endl;
            cin >> size;

            cout << "Please enter number of pepperoni toppings: " << endl;
            cin >> pepperoni;

            cout << "Please enter number of cheese toppings: " << endl;
            cin >> cheese;

            pizza.setPizzaParameters(type, size, pepperoni, cheese);

            PizzaOrder.push_back(pizza);

            orderIndex++;
        }

        // display the details of each pizza order on screen
        void displayOrderDetails(){
            double total_price = 0;
            for (int i = 0; i < orderIndex; i++){
                cout << "Displaying pizza " << i+1 << " order details" <<endl;
                PizzaOrder[i].outputDescription();
                total_price += PizzaOrder[i].getPrice();
            }
            cout << "The total price is " << total_price << "$" << endl;
        }
};

// define order, ask for X number of pizza orders, display details on screen
int main(){
    Order myOrder;
    myOrder.begin();
    myOrder.displayOrderDetails();
    return 0;
};