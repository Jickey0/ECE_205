/*
Write a program that will take a list of integers from the user via the console and calculate a 
histogram of the numbers entered. Use vectors and enter a negative integer to end the input. A 
histogram is a representation of the distribution of data based on which bin they fall into. A bin is a 
range of numbers determined based on data. For this problem, assume that bins are of size 10 (i.e., bin 
1 = 0…9, bin 2 = 10…19, etc). Output the histogram to the console as a list of bins and the number of 
values that fall into each one. Create functions as needed to have a modular design. Test your code 
with multiple inputs to ensure robustness.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

int main(){
    std::vector<int> numbers;
    std::vector<int> bins;

    cout << "Welcome to the Histogram Generator 1.00" << endl;
    cout << "Please enter integers (enter a negative integer to finish):" << endl;
    int input;
    while (true) {
        cin >> input;
        if (input < 0) {
            break; // Exit loop on negative input
        }
        numbers.push_back(input);
    }

    // sort into bins
    for (int i = 0; i < numbers.size(); i++) {
        int bin = numbers[i] / 10; // Determine which bin the number falls into
        if (bin >= bins.size()) {
            bins.resize(bin + 1); // Resize bins vector if needed
        }
        bins[bin]++; // Increment the count for the appropriate bin
    }

    // Output the histogram
    cout << "Histogram:" << endl;
    for (int i = 0; i < bins.size(); i++) {
        cout << "Bin " << i << " (" << i*10 << "-" << (i+1)*10 - 1 << "): " << bins[i] << endl;
    }
}