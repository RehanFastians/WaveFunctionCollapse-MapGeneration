#pragma once
#include <iostream>
#include"minHeap.hpp"
#include<functional>
#include<utility>
#include<vector>
#include "tile.hpp"
#include "cell.hpp"

class Grid
{
    const int gridDim = 800; // 2-Dimensional
    int numTile = 0;
    bool isPaused = false;
    bool isHomeScreen = true;
    std::vector<Tile> tiles;
    std::vector<std::vector<Cell>> cells;
    MinHeap<std::pair<int,int>> entropyMinHeap;
    int collapsedCount =0;

    void loadSockets(std::string type);
    void draw();                            // Draw the board
    bool isCompeleteCollapsed();            // To check if the map has been compeleted
    void restart();     // Restart generation after contradiction
    void propagate(int startX, int startY);
    void process();
    void showHomeScreen();
public:
    Grid(int numTile);                      // Contructor to initialize entropy
    void generateMap(); // Generate Map
};