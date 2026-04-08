#include <iostream>
using namespace std;

// shit doesn't work! What can we do? 
int main() {
  // Add is not defined yet
  Add(1, 2);
}

int Add(int x, int y) {
  return x + y;
}

// even worse is when we have circular dependacies! 
void HandlePositive(int x) {
  if (x < 0) HandleNegative(x);
}

void HandleNegative(int x){
  if (x >= 0) HandlePositive(x);
}
// moving jit around won't fix it! 

// WE USE PROTOTYPES
// normal function example: 
int Add(int x, int y) {
  return x + y;
}

// prototype example
int Add(int x, int y);

// thats sick right!?!

// example in action -->
int Calculate();

int main() {
  int x { Calculate() };
}

int Calculate() {
  return 10 * 10;
}
