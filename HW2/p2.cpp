#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

int main() {
    // Open the input file
    ifstream inputFile("testscores.txt");

    string line;
    float average = 0.0;
    float standardDeviation = 0.0;

    // Read each line, convert to float, and calculate average
    cout << "File Content: " << endl;
    while (getline(inputFile, line)) {
        float num = std::stof(line);
        average += num;
    }

    average = average / 5;

    // Rewind file
    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    // calculate standard deviation
    while (getline(inputFile, line)) {
        float num = std::stof(line);
        standardDeviation += (num - average) * (num - average);
    }

    standardDeviation = sqrt(standardDeviation / (4));

    // round to 2 decimal places
    average = floorf(average * 100) / 100;
    standardDeviation = floorf(standardDeviation * 100) / 100;

    // print results
    cout << "Average: " << average << endl;
    cout << "Standard Deviation: " << standardDeviation << endl;

    ofstream file("results.txt");
    file << "Average: " << average << "\n";
    file << "Standard Deviation: " << standardDeviation << "\n";
    file.close();

    // Close the file
    inputFile.close();
    return 0;
}