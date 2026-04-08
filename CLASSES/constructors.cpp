// 1. Implicitly Generated Default Constructor If no constructor is defined, the compiler provides a default constructor:

class Example {
public:
// No constructor explicitly defined
};

int main() {
    Example obj; // Compiler-generated default constructor is called
    return 0;
}

// 2. Explicitly Defined Default Constructor A programmer can define a default constructor explicitly:

class Example {
    public:
        Example() {
            std::cout << "Default constructor called!" << std::endl;
        }
};

int main() {
    Example obj; // Calls the user-defined default constructor
    return 0;
}

// 3. Default Constructor with Initialization Default constructors can initialize class members:

class Example {
    int value;
    public:
        Example() : value(10) {
            std::cout << "Value initialized to " << value << std::endl;
        }
};

int main() {
    Example obj; // Initializes `value` to 10
    return 0;
}