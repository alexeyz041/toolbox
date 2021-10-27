
#include <memory>
#include <iostream>
#include <ostream>


using namespace std;

class A {
public:
    A(int n1) {
        n = n1;
        cout << "A " << n << endl;
    }

    ~A() {
        cout << "~A " << n << endl;
    }

private:
    int n;
};


shared_ptr<A> get()
{
    shared_ptr<A> c = make_shared<A>(3);
    return c;
}


int main()
{
shared_ptr<A> a(new A(1));
shared_ptr<A> b = make_shared<A>(2);
shared_ptr<A> c = get();
    cout << "main" << endl;
}
