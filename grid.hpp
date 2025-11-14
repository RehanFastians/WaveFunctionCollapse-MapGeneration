#pragma once
#include <iostream>
#include <vector>
#include <tile.hpp>

class Grid
{
    const int gridDim = 800; // 2-Dimensional
    int numTile = 0;
    std::vector<Tile> tiles;

public:
    Grid(int numTile);
};