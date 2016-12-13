#include <iostream>
#include <string>
#include "vector.hpp"

int main()
{
    Vector v1(1,2);
    Vector v2(3,4);
    std::cout << "length = " << Vector::dist(v1, v2) << "!\n";
}

