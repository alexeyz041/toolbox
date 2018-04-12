//
// https://docs.fabric.io/apple/crashlytics/test-crash.html#cause-a-test-crash
//
// Fun fact: Divide-by-zero is illegal on i386 and x84-64, but is a valid operation on ARM.
//  Dividing by zero will cause crashes in the simulator, but not on iOS devices.



#include <iostream>

int main(int argc, char *argv[])
{
    int Dividend = 10;
    int Divisor = 0;

    try {
        std::cout << Dividend / Divisor;
    } catch(...) {
        std::cout << "Error /0";
    }
    return 0;
}
