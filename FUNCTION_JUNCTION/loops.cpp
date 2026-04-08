// simple while loop
int i { 0 };
while (i <= 5) {
  ++i;
}

// STUPID do while loop
do {
  // Body here
} while (shouldLoop);


// for loops AKA the goat of all loops
for (initialize; condition; update) {
  // Code here
}

// example: 
int main(){
  for (int Number{1}; Number <= 10; ++Number) {
    cout << Number << ", ";
  }
}

// we can even do this: but its prob better with a while
int Number { 1 };
for ( ; Number < 10 ; ) {
  cout << Number << ", ";
  ++Number;
}

// note on continue statments
continue;
// that's it
