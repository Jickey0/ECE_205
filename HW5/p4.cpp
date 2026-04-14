#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

class PrimeNumber{
    private: 
        int number;

    public:
        PrimeNumber(){
            number = 1;
        }

        
        int setPrimeNumber(int number_input){
            if (isPrime(number_input)){
                number = number_input;
            }
            else {
                cout << "Please enter valid prime number" << endl;
            }
            return 0;
        }

        int getPrimeNumber(){
            return number;
        }

        // increase to the next highest prime number and return new object
        PrimeNumber operator++() {
            PrimeNumber myNewPrimeNumber;
            int num = number + 1;

            while (!isPrime(num))
                num++;

            myNewPrimeNumber.setPrimeNumber(num);
            return myNewPrimeNumber;
        }

        // decrement to the next lowest prime number and return new object
        PrimeNumber operator--() {
            PrimeNumber myNewPrimeNumber;
            int num = number - 1;

            while (num > 1 && !isPrime(num))
                num--;

            myNewPrimeNumber.setPrimeNumber(num);
            return myNewPrimeNumber;
        }

        // checks if the number is prime (O^2 time complexity :( )
        bool isPrime(int num){
            for (int i = 0; i < num ; i++){
                for (int j = 0; j < num ; j++)
                    if (i * j == num) { // if any two numbers less than the prime number itself can be multiplied to get the number we are checking then it is not a prime number
                        return false;
                    }
            }
            return true;
        }
};

int main(){
    cout << "Creating a prime number which defaults to 1" << endl;
    PrimeNumber myPrimeNumber;
    PrimeNumber newPrimeNumber;

    cout << "Result: " << myPrimeNumber.getPrimeNumber() << endl;

    cout << "Setting value to 12 (an invalid input)" << endl;
    myPrimeNumber.setPrimeNumber(12);

    cout << "Setting value to 13" << endl;
    myPrimeNumber.setPrimeNumber(13);
    cout << "Result: " << myPrimeNumber.getPrimeNumber() << endl;

    cout << "Creating a increased prime number using ++ operation" << endl;
    newPrimeNumber = myPrimeNumber.operator++();
    cout << "new Prime Number Result: " << newPrimeNumber.getPrimeNumber() << endl;

    cout << "Creating a decreased prime number using -- operation" << endl;
    newPrimeNumber = myPrimeNumber.operator--();
    cout << "new Prime Number Result: " << newPrimeNumber.getPrimeNumber() << endl;
}