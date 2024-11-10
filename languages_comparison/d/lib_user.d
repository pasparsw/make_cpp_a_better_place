import std.stdio;

extern (C++)
{
    struct Point
    {
        int x, y;
    }
    int Add(const int a, const int b);
    bool IsEqual(T)(const T a, const T b);
}

void main()
{
    Point p1 = Point(12, 24);
    Point p2 = Point(36, 48);
    const int adding_result = Add(p1.x, p2.x);
    const bool comparison_result = IsEqual(p1, p2);
    
    writeln("adding_result = ", adding_result);
    writeln("comparison_result = ", comparison_result);
}