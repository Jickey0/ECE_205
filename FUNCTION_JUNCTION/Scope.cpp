#include <iostream>
using namespace std;

// function returns 2 using block scope
int GetInt() {
  int x{1};
  { x++; }
  return x;
}

// This code is invalid
int GetInt() {
  {
    int x{1};
    x++;
  }
  return x;
}

// shadowed variables --> inner variables "hides" outer variables
int main(){
  int x{1};
  {
    // This 'x' shadows the outer 'x'
    int x{2};
    cout << "Inner x: " << x; // Outputs 2
  }
  cout << "\nOuter x: " << x; // Outputs 1
}

