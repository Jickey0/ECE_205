#include <iostream>

using namespace std;

int main(){
  switch (4) {
  case 1:
    cout << "Monday";
  case 2:
    cout << "Tuesday";
    case 3:
    cout << "Wednesday";
    break;
  default:
    cout << "Something else";
  }
}