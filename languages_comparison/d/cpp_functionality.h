// cpp_functionality.h
#pragma once
// example C++ struct to import
struct Point
{
    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }
    int x, y;
};
// ordinary function
int Add(const int a, const int b);
// template function
template <typename T>
bool IsEqual(const T a, const T b)
{
    return a == b;
}

template<>
bool IsEqual<Point>(const Point a, const Point b)
{
    return a == b;
}
