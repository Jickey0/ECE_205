#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main() {
    // finalist count
    int finalist_count = 25;
    bool finalist_picked = false;
    int finalists[4] = {-1, -1, -1, -1};

    while (finalist_picked == false) {
        // Generate a random number between 0 and 25 using
        int value = rand() % (finalist_count + 1);

        for (int i = 0; i < 4; i++) {
            if (finalists[i] == value) {
                break;
            }
            if (finalists[i] == -1) {
                finalists[i] = value;
                if (i == 3) {
                    finalist_picked = true;
                }
                break;
            }
        }
    }

    // print results
    cout << "The 4 finalists are: " << endl;
    for (int i = 0; i < 4; i++) {
        cout << finalists[i] << endl;
    }
}