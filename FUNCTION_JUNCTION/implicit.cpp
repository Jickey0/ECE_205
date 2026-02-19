#include <iostream>
using namespace std;

// Converting 0 to a boolean results in false
bool IsDeadFrom0() {
  return 0;  // false
}

// Converting a non-0 integer to a
// boolean results in true
bool IsDeadFrom42() {
  return 42; // true
}

// Converting false to an integer results in 0
int LevelFromFalse() {
  return false; // 0
}

// Converting true to an integer results in 1
int LevelFromTrue() {
  return true; // 1
}

// Conversions can happen multiple times
// This function will convert 50 to true...
bool GetHealth() {
  return 50;
}

// This variable initialization converts true to 1
int Health { GetHealth() }; // Health will be 1

// narrowing casts --> 3.14 to 3
int main() {
  int Pi = 3.14; 
}

