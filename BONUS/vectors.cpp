#include <iostream>
#include <vector>   // 1. Always include the vector header
#include <algorithm> // For sorting and other algorithms

int main() {
    // --- 2. INITIALIZATION ---
    // Creating an empty vector of integers
    std::vector<int> numbers;

    // Initializing with specific values (C++11 and later)
    std::vector<int> primes = {2, 3, 5, 7, 11};

    // Initializing with a specific size and default value (5 elements, all 100)
    std::vector<int> hundreds(5, 100);


    // --- 3. ADDING & REMOVING ELEMENTS ---
    // Add to the end
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    // Remove the last element
    numbers.pop_back(); 


    // --- 4. ACCESSING ELEMENTS ---
    // Using square brackets (No bounds checking - faster)
    int first = primes[0];

    // Using .at() (Bounds checking - throws an error if index is invalid)
    try {
        int third = primes.at(2);
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range error: " << e.what() << std::endl;
    }

    // Access front and back directly
    int frontElement = primes.front();
    int backElement = primes.back();


    // --- 5. SIZE & CAPACITY ---
    // Number of elements currently in the vector
    size_t size = numbers.size();

    // Check if empty
    bool isEmpty = numbers.empty();

    // Capacity is how much memory is allocated; Size is how much is used
    size_t capacity = numbers.capacity();


    // --- 6. ITERATING (LOOPING) ---
    // Modern Range-based for loop (Recommended)
    std::cout << "Primes: ";
    for (int p : primes) {
        std::cout << p << " ";
    }
    std::cout << std::endl;

    // Traditional for loop using index
    for (size_t i = 0; i < numbers.size(); i++) {
        // numbers[i]...
    }

    // Using Iterators
    for (auto it = primes.begin(); it != primes.end(); ++it) {
        // *it accesses the value
    }


    // --- 7. USEFUL MODIFIERS ---
    // Insert at a specific position (Inserting '15' at the beginning)
    primes.insert(primes.begin(), 15);

    // Erase a specific element (Erasing the first element)
    primes.erase(primes.begin());

    // Clear everything
    hundreds.clear();


    // --- 8. USEFUL ALGORITHMS (Requires <algorithm>) ---
    // Sorting
    std::sort(primes.begin(), primes.end());

    // Reversing
    std::reverse(primes.begin(), primes.end());

    return 0;
}