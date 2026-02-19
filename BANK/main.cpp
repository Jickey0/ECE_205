#include <iostream>
#include <cstdint> // USE THIS HEADER FOR FIXED WIDTH INTEGERS

using namespace std;

int main() {
    // Integer types
    int8_t SmallNumber{100};
    int16_t MediumNumber{10'000};
    int32_t LargeNumber{1'000'000'000};
    int64_t HugeNumber{1'000'000'000'000'000'000};

    // These are fine
    int8_t NegativeNumber{-100};
    int8_t PositiveNumber{100};

    // Error: This is out of range
    //int8_t LargerNumber{200}; 

    // weird error: overflow! neg number causes wrap around
    int Signed{-1};
    unsigned int Unsigned{1};

    if (Signed > Unsigned) {  // true
        cout << Signed << " is greater than "
        << Unsigned << "?";
    }

    // floating point types
    float A{3.14};
    double B{9.8};
    long double C{1.6180339887};

    // Cause cout to show more decimal places
    // when outputting floating point numbers
    cout.precision(16);

    float A { 1.1111111111111111 };
    cout << "Float Precision:  "
    << A + A << '\n';
}