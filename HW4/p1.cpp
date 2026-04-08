#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector>  // used to create a dynamic array for user input

using namespace std;


/*
Write a program that will take a list of integers from the user via the console and calculate a histogram 
of the numbers entered. Assume that the size of the array will be entered by the user first, followed by a list of 
non-negative integers. A histogram is a representation of the distribution of data based on which bin they fall into. 
A bin is a range of numbers determined based on data. For this problem, assume that bins are of size 10 (i.e. bin 1 
= 0…9, bin 2 = 10…19, etc). Output the histogram to the console as a list of bins and number of values that fall 
into each one. Use array to store the user input and create functions as needed to create a modular design. Test 
your code with multiple inputs to ensure robustness.
*/

void requestInputArray(int inputArr[], int size);
void displayHistogram(int HistogramArr[], int size);

int main() {
    cout << "Enter the size of the array: ";
    int size;
    cin >> size;
    std::vector<int> inputArr(size);  // Use vector for dynamic sizing
    std::vector<int> HistogramArr(size);

    requestInputArray(inputArr.data(), size);  // Pass pointer to vector's internal array

    for (int i = 0; i < size; i++) {
        HistogramArr[i] = 0;
    }

    for (int i = 0; i < size; i++) {
        HistogramArr[inputArr[i]] += 1;
    }

    displayHistogram(HistogramArr.data(), size);

    return 0;
}

void displayHistogram(int HistogramArr[], int size){
    cout << "Histogram" << endl;

    for (int i = 0; i < size; i++) {
        cout << "Bin " << i*10 << "-" << (i+1)*10 - 1 << ": " << HistogramArr[i] << endl;
    }
}

void requestInputArray(int inputArr[], int size) {
    cout << "Enter " << size << " non-negative integers: " << endl;
    for (int i = 0; i < size; i++) {
        cin >> inputArr[i];

        if (inputArr[i] < 0) {
            cout << "Negative numbers are not allowed :(" << endl;
            i--; // repeat iteration
        }
    }
}
