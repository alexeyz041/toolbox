
#include <memory>
#include <iostream>
#include <ostream>


using namespace std;

class A {
public:
    A() {
        cout << "A " << endl;
    }

    ~A() {
        cout << "~A " << endl;
    }

    void fA() {
        cout << "fA()" << endl;
    }
};


class B {
public:
    B(shared_ptr<A> a0) : a(a0) {
        cout << "B " << endl;
    }

    ~B() {
        cout << "~B" << endl;
    }

    void fB() {
        cout << "fB()" << endl;
        a->fA();
    }

private:
    shared_ptr<A> a;
};


int main()
{
shared_ptr<A> a = make_shared<A>();
unique_ptr<B> b = make_unique<B>(a);
    cout << "main" << endl;
    b->fB();
}
