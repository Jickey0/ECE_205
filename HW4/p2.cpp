#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector>  // used to create a dynamic array for user input

using namespace std;


/*
Write a program for judging a diving competition. The program will take as input a degree of difficulty 
and 7 judges’ scores from the console (store in array). The scores are between 0 and 10 and can be a real number. 
The highest and lowest scores input are thrown out and the remaining scores are added together. The sum is then 
multiplied by the degree of difficulty of a dive, which ranges from 1.2 to 3.8 points. The result is then multiplied 
by 0.6 to determine the diver’s score. Test your code with multiple inputs and ensure that the program forces all 
inputs to be within the allowable data ranges. 
*/
float calculateScore(int degreeOfDifficulty, vector<double> judgesScores);

int main(){
    float score;

    double degreeOfDifficulty;
    vector<double> judgesScores(7); // create a vector to store the judges' scores

    // Get degree of difficulty from user
    cout << "Enter the degree of difficulty (1.2 to 3.8): ";
    cin >> degreeOfDifficulty;

    // Validate degree of difficulty
    while(degreeOfDifficulty < 1.2 || degreeOfDifficulty > 3.8){
        cout << "Degree of difficulty must be between 1.2 and 3.8: ";
        cin >> degreeOfDifficulty;
    }

    // Get judges' scores from user
    for(int i = 0; i < 7; i++){
        cout << "Enter score from judge " << i + 1 << " (0 to 10): ";
        cin >> judgesScores[i];

        // Validate judges' scores
        while(judgesScores[i] < 0 || judgesScores[i] > 10){
            cout << "Score must be between 0 and 10: ";
            cin >> judgesScores[i];
        }
    }

    score = calculateScore(degreeOfDifficulty, judgesScores);

    //display score
    cout << "Your final score is: " << score;
}

float calculateScore(int degreeOfDifficulty, vector<double> judgesScores){
    float minScore = judgesScores[0];
    float maxScore = judgesScores[0];
    float sum = 0;

    for (int i = 0; i < 7; i++){
        if (judgesScores[i] < minScore){
            minScore = judgesScores[i];
        }
        if (judgesScores[i] > maxScore){
            maxScore = judgesScores[i];
        }
        sum += judgesScores[i];
    }

    return degreeOfDifficulty * 0.6 * sum;
}