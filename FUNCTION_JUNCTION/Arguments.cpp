#include <iostream>
using namespace std;

int Health{150};

void TakeDamage(int Damage){
  cout << "Inflicting " << Damage << " Damage";
  Health -= Damage;
  cout << " - Health: " << Health << '\n';
}

int main(){
  TakeDamage(20);
  TakeDamage(50);
  TakeDamage(70);
}

// or with multiple parameters
int Multiply(float x, float y) {
  return x * y;
}

int Result { Multiply(2, 2.5) };

// default arguments
void TakeDamage(
  int Damage, bool isMagical = false
) {
  // Magical damage is doubled
  Health -= isMagical ? Damage * 2 : Damage;
}

// This still works, but the false isn't needed
TakeDamage(50, false);

// It is equivalent to this:
TakeDamage(50);

// To override the default parameter:
TakeDamage(50, true);