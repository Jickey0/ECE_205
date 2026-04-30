#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Function to delete repeated characters
void deleteRepeatedChars(vector<char>& myVector) {
    for (int i = 0; i < myVector.size(); i++) {
        for (int j = i + 1; j < myVector.size(); ) {
            if (myVector[i] == myVector[j]) {
                myVector.erase(myVector.begin() + j);
            } else {
                j++;
            }
        }
    }
}

// Function to perform Selection Sort in descending order (z to a)
void selectionSort(vector<char>& myVector) {
    int n = myVector.size();
    for (int i = 0; i < n - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < n; j++) {
            // Finding the largest character for descending order
            if (myVector[j] > myVector[maxIndex]) {
                maxIndex = j;
            }
        }
        // Swap the found maximum element with the first element of the unsorted part
        swap(myVector[i], myVector[maxIndex]);
    }
}

// prompt the user to enter characters until they enter '0' to stop
void askForInput(vector<char>& myVector) {
    char letter;
    while (true) {
        cout << "Enter a character (or '0' to stop): ";
        cin >> letter;
        if (letter == '0') break;
        myVector.push_back(letter);
    }
}

// display the contents of the vector
void displayVector(const vector<char>& myVector) {
    cout << "Vector contains: ";
    for (char c : myVector) {
        cout << c << " ";
    }
    cout << endl;
}

int main() {
    vector<char> myVector;

    askForInput(myVector);
    
    cout << "\nOriginal input:" << endl;
    displayVector(myVector);

    deleteRepeatedChars(myVector);
    cout << "\nAfter removing duplicates:" << endl;
    displayVector(myVector);

    selectionSort(myVector);
    cout << "\nAfter sorting (Descending):" << endl;
    displayVector(myVector);

    return 0;
}