#include <iostream>
#include "grid.hpp"

int main()
{
    Grid map(20);
    SetTargetFPS(120);
    map.generateMap();
    return 0;
}