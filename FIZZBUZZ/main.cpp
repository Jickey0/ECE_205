#include <iostream>
using namespace std;

int main(){
    for (int number {1}; number < 30; ++number){
        bool fizz { number % 3 == 0};
        bool buzz { number % 5 == 0};

        if (fizz && buzz)
            cout << "FizzBuzz";
        else if (fizz)
            cout << "Fizz";
        else if (buzz)
            cout << "Buzz";
        else
            cout << number;

        cout << '\n';
    }
}