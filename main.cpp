#include <iostream>
#include "grid.hpp"

int main()
{
    InitWindow(800, 800, "Sample window");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }
    return 0;
}