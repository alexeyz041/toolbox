
#include <iostream>
#include <string>
#include <sstream>
#include <map>


class Obj {
public:

    void method1(double p)
    {
	std::cout << "I am method 1 with p = " << p << std::endl;
    }

    void method2(double p)
    {
	std::cout << "I am method 2 with p = " << p << std::endl;
    }

    void method3(double p)
    {
	std::cout << "I am method 3 with p = " << p << std::endl;
    }

};


std::map<std::string,void (Obj::*)(double)> fn_map = {
    { "method1", &Obj::method1 },
    { "method2", &Obj::method2 },
    { "method3", &Obj::method3 }
};



int main()
{
Obj o;

    o.method1(1);
    o.method2(2);
    o.method3(3);

    for(int i=1; i < 4; i++) {
	std::stringstream name;
	name << "method" << i;
	std::string n = name.str();

	void (Obj::*method)(double) = fn_map[n];
	(o.*method)(i+5);
    }

    return 0;
}
