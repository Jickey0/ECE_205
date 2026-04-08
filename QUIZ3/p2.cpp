/* array of user scores 0-100
input ends when user enters negative num
above 100 is ignored

functions:
find max score
find min score
find average
find standard deviation

display results
*/ 

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

int getUserInputScores(int scores[]){
    int count = 0;
    cout << "Enter scores (negative number to end): " << endl;
    while (true) {
        cin >> scores[count];
        if (scores[count] < 0) {
            break; // End input on negative number
        }
        if (scores[count] > 100) {
            cout << "Score must be between 0 and 100. Try again." << endl;
            continue; // Ignore scores above 100
        }
        count++;
    }
    return count;
}
    
int findMaxScore(int scores[], int count){
    int maxScore = 0;
    for (int i = 0; i < count; i++) {
        if (scores[i] > maxScore) {
            maxScore = scores[i];
        }
    }
    return maxScore;
}

int findMinScore(int scores[], int count){
    int minScore = 100;
    for (int i = 0; i < count; i++) {
        if (scores[i] < minScore) {
            minScore = scores[i];
        }
    }
    return minScore;
}

double findAverage(int scores[], int count){
    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += scores[i];
    }
    return sum / count;
}

double findStandardDeviation(int scores[], int count, double average){
    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += (scores[i] - average)*(scores[i] - average);
    }
    return sqrt(sum / count);
}


int main(){
    int scores[100]; // Assuming a maximum of 100 scores
    int count = 0;
    int maxScore = 0;
    int minScore = 100;
    double sum = 0;
    double average = 0;

    count = getUserInputScores(scores);

    maxScore = findMaxScore(scores, count);
    minScore = findMinScore(scores, count);
    average = findAverage(scores, count);
    double stdDev = findStandardDeviation(scores, count, average);

    // display results
    cout << "Max Score: " << maxScore << endl;
    cout << "Min Score: " << minScore << endl;
    cout << "Average Score: " << average << endl;
    cout << "Standard Deviation: " << stdDev << endl;
}