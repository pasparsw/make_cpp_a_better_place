#include <iostream>
#include "cpp_functionality.h"

int main(int argc, char** argv)
{
    Point p1 {12, 24};
    Point p2 {36, 48};
    const int adding_result = Add(p1.x, p2.x);
    const bool comparison_result = IsEqual(p1, p2);

    std::cout << "adding_result = " << adding_result << std::endl;
    std::cout << "comparison_result = " << comparison_result << std::endl;
}