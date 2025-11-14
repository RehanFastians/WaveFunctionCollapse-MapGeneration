#pragma once
#include <iostream>
#include <vector>
#include <tile.hpp>
#include <cell.hpp>

class Grid
{
    const int gridDim = 800; // 2-Dimensional
    int numTile = 0;
    std::vector<Tile> tiles;
    std::vector<std::vector<Cell>> cells;
public:
    Grid(int numTile);
};