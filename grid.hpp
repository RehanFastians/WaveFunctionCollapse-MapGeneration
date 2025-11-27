#pragma once
#include <iostream>
#include<queue>
#include<functional>
#include<utility>
#include<vector>
#include "tile.hpp"
#include "cell.hpp"

class Grid
{
    const int gridDim = 800; // 2-Dimensional
    int numTile = 0;
    std::vector<Tile> tiles;
    std::vector<std::vector<Cell>> cells;
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> entropyMinHeap;
    int collapsedCount =0;

    void loadSockets(std::string type);
    void draw();                            // Draw the board
    std::pair<int, int> findLeastEntropy(); // Finding the least entropy cell
    bool isCompeleteCollapsed();            // To check if the map has been compeleted
    void process();                         // Processes Entropy
    void processCell(int y, int x, std::queue<std::pair<int, int>> &bfs, std::vector<std::vector<bool>> &visit); // Update each cell and manage traversal
    void restart();     // Restart generation after contradiction

    void processtemp();
    void processCellTemp(int x, int y);

public:
    Grid(int numTile);                      // Contructor to initialize entropy
    void generateMap(); // Generate Map
};