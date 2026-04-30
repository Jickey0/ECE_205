#include <iostream>
#include <string>

int main() {
    // --- 1. THE ADDRESS-OF OPERATOR (&) ---
    int myScore = 100;
    
    // '&' gives us the memory address where the variable lives
    // Think of this as the "GPS coordinates" of myScore
    std::cout << "Value: " << myScore << std::endl;
    std::cout << "Address: " << &myScore << std::endl; 


    // --- 2. POINTER DECLARATION (*) ---
    // A pointer is a variable that STORES a memory address.
    // The type (int*) must match the type of data it points to.
    int* ptr = &myScore; 

    std::cout << "Pointer Value (Address): " << ptr << std::endl;


    // --- 3. DEREFERENCING (*) ---
    // When used on an existing pointer, '*' means "go to this address and get the value"
    std::cout << "Value at Pointer: " << *ptr << std::endl;

    // You can also change the value of the original variable through the pointer
    *ptr = 150; 
    std::cout << "New value of myScore: " << myScore << std::endl;


    // --- 4. NULL POINTERS ---
    // Always initialize pointers. If you don't have an address yet, use nullptr.
    int* uninitializedPtr = nullptr;

    if (uninitializedPtr == nullptr) {
        std::cout << "Pointer is currently pointing to nothing safely." << std::endl;
    }


    // --- 5. REFERENCES (& in type declaration) ---
    // A reference is an ALIAS. It's not a separate object with its own address;
    // it's just another name for the same variable.
    int& ref = myScore;
    ref = 200; // Changing ref changes myScore directly

    std::cout << "myScore after ref change: " << myScore << std::endl;


    // --- 6. POINTERS VS REFERENCES ---
    /*
        POINTERS:
        - Can be reassigned to point to something else.
        - Can be null (nullptr).
        - Use * to get the value (dereference).

        REFERENCES:
        - Cannot be reassigned (stuck to the original variable for life).
        - Cannot be null.
        - Easier to use (no * needed to access value).
    */


    // --- 7. DYNAMIC MEMORY (new / delete) ---
    // Using 'new' allocates memory on the "Heap" instead of the "Stack"
    int* dynamicInt = new int(42); 

    std::cout << "Dynamic Value: " << *dynamicInt << std::endl;

    // CRITICAL: You must manually free memory allocated with 'new'
    delete dynamicInt; 
    dynamicInt = nullptr; // Good practice to prevent "dangling pointers"

    return 0;
}