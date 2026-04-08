#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector> 

using namespace std;

// Define a class called Book which is an abstract data type for searching a book from a history bookstore. 
// You will search the book by category, publishing year, and ISBN. 
// The book category should be a string, publishing year and ISBN should be an integer type. 
// The book will only be available in the store if the book was published after the year 1950 and the category is “history” for any ISBN value. 
// Display the output if the book is available or not. 
// You will take the input from the console. 

// Jack Hickey - ECE 205 - Quiz 4 - 6/5/2024

// 1. Make all member variables private (category, publishing year, and ISBN).
class Book {
    private:
        string category;
        int publishingYear;
        int ISBN;

    
    public:
        // 2. A constructor to set the book selecting parameters as an argument (category, publishing year, and ISBN)
        Book(string c, int y, int i) {
            category = c;
            publishingYear = y;
            ISBN = i;
        }

        // 4. Other member/non-member functions if you need them. (findBook uses the parameters to determine if the book is available or not)
        int findBook(){
            if (publishingYear > 1950 && (category == "history" || category == "History")) { 
                return 1; 
            } else {
                return 0;
            }
        }
};

// 3. A non-member output function that outputs the availability
void displayAvailability(Book myBook) {
    if (myBook.findBook()) {
        cout << "The book is available :)" << endl;
    } else {
        cout << "The book is not available :(" << endl;
    }
}

int main(){
    cout << "Enter book category: ";
    string category;
    cin >> category;
    cout << "Enter publishing year: ";
    int year;
    cin >> year;
    cout << "Enter ISBN: ";
    int isbn;
    cin >> isbn;

    // define our book based on the user inputs
    Book myBook(category, year, isbn);

    // Display the availability based on the parameters
    displayAvailability(myBook);

    return 0;
}
