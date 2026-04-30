/*
 A prime number is an integer greater than 1 and divisible only by itself and 1. Implement the 
Sieve of Erathosthenes algorithm to find all prime integers less than a user-entered integer N. More 
information on the algorithm: https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes . Use vectors in 
your implementation and output all prime numbers calculated to the console.
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// uses the Erathosthenes algorithm to find all prime integers
void ErathosthenesSortsWithHisSieve(vector<int>& myVector){
    for (int p = 2; p*p < myVector.size(); p++) { // check if p^2 < N
        for(int j = p*p; j < myVector.size(); j = j + p){
            myVector[j] = 0;
        }
    }
}

// display the contents of the vector
void displayVector(const vector<int>& myVector) {
    cout << "Vector contains: ";
    for (int i = 0; i < myVector.size(); i++) {
        if (myVector[i] == 1){
            cout << i << " ";
        }
    }
    cout << endl;
}

int main(){
    int N;
    cout << "Please enter a int N to find all prime numbers between 0-N: ";
    cin >> N;

    vector<int> sieve(int(N), 1);

    // remove first two numbers as they are not primes but will not be accounted for in the alg
    sieve[0] = 0;
    sieve[1] = 0;

    ErathosthenesSortsWithHisSieve(sieve);

    cout << "The prime numbers are as follows: " << endl;
    displayVector(sieve);
}