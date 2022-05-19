
#include <memory>
#include <iostream>
#include <ostream>
#include <tuple>


using namespace std;

class A {
public:
    void fn(size_t x) { cout << "fnA" << x << endl; }
};

class B {
public:
    void fn(size_t x) { cout << "fnB" << x << endl; }
};

class C {
public:
    void fn(size_t x) { cout << "fnC" << x << endl; }
};

template <size_t I = 0, typename... Ts>
constexpr void printTuple(tuple<Ts...> tup, size_t X)
{
    if constexpr(I == sizeof...(Ts))
    {
        return;
    }
    else
    {
        get<I>(tup).fn(X);
        printTuple<I + 1>(tup, X);
    }
}

class D {
public:
    A a;
    B b;
    C c;
    tuple<A,B,C> t = make_tuple(a, b, c);

   D() {
      printTuple(t, 1);
   }
};

int main()
{
    D d;
    return 0;
}
