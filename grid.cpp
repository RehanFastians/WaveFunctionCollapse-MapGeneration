#include "grid.hpp"
#include <string>
#include <cstdlib>
#include <ctime>

Grid::Grid(int numTile)
{
    // Seeding rand() function

    srand(time(0));

    //  Initializing window for showing generated map

    InitWindow((float)gridDim, (float)gridDim, "Wave-Function-Collapse");

    this->numTile = numTile;

    // Setting sockets manually

    std::vector<std::vector<std::string>> tempSockets(13, std::vector<std::string>(4));
    tempSockets[0] = {"AAA", "AAA", "AAA", "AAA"};
    tempSockets[1] = {"BBB", "BBB", "BBB", "BBB"};
    tempSockets[2] = {"BBB", "BCB", "BBB", "BBB"};
    tempSockets[3] = {"BBB", "BDB", "BBB", "BDB"};
    tempSockets[4] = {"ABB", "BCB", "BBA", "AAA"};
    tempSockets[5] = {"ABB", "BBB", "BBB", "BBA"};
    tempSockets[6] = {"BBB", "BCB", "BBB", "BCB"};
    tempSockets[7] = {"BDB", "BCB", "BDB", "BCB"};
    tempSockets[8] = {"BDB", "BBB", "BCB", "BBB"};
    tempSockets[9] = {"BCB", "BCB", "BBB", "BCB"};
    tempSockets[10] = {"BCB", "BCB", "BCB", "BCB"};
    tempSockets[11] = {"BCB", "BCB", "BBB", "BBB"};
    tempSockets[12] = {"BBB", "BCB", "BBB", "BCB"};

    // Setting tiles

    tiles.reserve(13 * 4);
    for (int i = 0; i < 13; i++)
    {
        std::string path = "Images/";
        path += std::to_string(i);
        path += ".png";
        for (int angle = 0; angle < 4; angle++)
            tiles.emplace_back(path, tempSockets[i], angle);
    }

    // Intitalizing default entropy to the cells

    for (int i = 0; i < numTile; i++)
    {
        std ::vector<Cell> temp;
        for (int j = 0; j < numTile; j++)
            temp.push_back(Cell(tiles.size()));
        cells.push_back(temp);
    }
}

void Grid::draw()
{
    // Just draws the updated board
    collapsedCount = 0;
    const int tileSize = gridDim / numTile;
    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() == 0)
                throw("Contradiction encountered in Wave Function Collapse"); // Handle contradiction
            if (cells[y][x].entropy.size() == 1)
            {
                tiles[cells[y][x].entropy[0]].draw(y, x, tileSize); // Draw the tile
                collapsedCount++;
            }
        }
    }
}

std::pair<int, int> Grid ::findLeastEntropy()
{

    int leastEntropy = tiles.size();

    // Find least Entropy
    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() < leastEntropy && cells[y][x].entropy.size() != 1)
            {
                leastEntropy = cells[y][x].entropy.size();
            }
        }
    }

    // Find the random occcurence of the least entropy value cell

    std::vector<std ::pair<int, int>> cellPosition;

    for (int x = 0; x < numTile; x++)
    {
        for (int y = 0; y < numTile; y++)
        {
            if (cells[y][x].entropy.size() == leastEntropy)
            {
                cellPosition.push_back({y, x});
            }
        }
    }
    return cellPosition[rand() % cellPosition.size()];
}

bool Grid::isCompeleteCollapsed()
{

    // This function checks if the map is compeletely collapse or not
    return numTile * numTile == collapsedCount;
    // for (int y = 0; y < numTile; y++)
    // {
    //     for (int x = 0; x < numTile; x++)
    //     {
    //         if (cells[y][x].entropy.size() != 1)
    //             return false;
    //     }
    // }
    // return true;
}

void Grid::processCell(int y, int x, std::queue<std::pair<int, int>> &bfs, std::vector<std::vector<bool>> &visit)
{
    // These directions idx are in congruence with the directions index in tiles checking function

    int diry[] = {-1, 0, 1, 0};
    int dirx[] = {0, 1, 0, -1};

    for (int j = 0; j < cells[y][x].entropy.size(); j++)
    {
        bool shouldExist = true; // This tells if this entropy value should exist by checking adjacent values

        for (int direction = 0; direction < 4; direction++)
        {
            // Adjacent cells position calculation

            int nextX = x + dirx[direction];
            int nextY = y + diry[direction];

            if (nextX < 0 || nextY < 0 || nextX >= numTile || nextY >= numTile) // Validating position
                continue;

            bool doesExist = false; // It tells if a valid neighbour exist
            for (int i = 0; i < cells[nextY][nextX].entropy.size(); i++)
            {
                if (tiles[cells[y][x].entropy[j]].isPossible(tiles[cells[nextY][nextX].entropy[i]], direction))
                {
                    doesExist = true;
                    break;
                }
            }
            if (!doesExist)
            {
                shouldExist = false;
                break;
            }

            if (visit[nextY][nextX])
                continue; // If the cell is visited dont visit it again obviously
            visit[nextY][nextX] = true;
            bfs.push({nextY, nextX}); // Push this for the upcoming traversal
        }

        // Collapse if no valid neighbour was found

        if (!shouldExist)
        {
            cells[y][x].collapse(j);
            j--;
        }
    }
}

void Grid::process()
{

    // Finding least Entropy and will collapse it

    std ::pair<int, int> startCell = findLeastEntropy();

    // Randomly collapsing the cell

    cells[startCell.first][startCell.second].randomCollapse();

    // Direction

    int diry[] = {-1, 0, 1, 0};
    int dirx[] = {0, 1, 0, -1};

    // queue to store the next cell whose entropy is to collapse and visit to keep track of visited to avoid repitition

    std::queue<std::pair<int, int>> bfs;
    std::vector<std::vector<bool>> visit(numTile, std::vector<bool>(numTile, false));

    // Push all adjacent cells of  the currently collapsed cell

    for (int direction = 0; direction < 4; direction++)
    {
        int nextX = startCell.second + dirx[direction];
        int nextY = startCell.first + diry[direction];
        if (nextX < 0 || nextY < 0 || nextX >= numTile || nextY >= numTile)
            continue;
        bfs.push({nextY, nextX});
        visit[nextY][nextX] = true;
    }

    // Process all cells (whole grid)

    while (!bfs.empty())
    {
        auto [y, x] = bfs.front();
        bfs.pop();

        processCell(y, x, bfs, visit);
    }
};

void Grid::restart()
{
    cells = {};

    // Intitalizing default entropy to the cells

    for (int i = 0; i < numTile; i++)
    {
        std ::vector<Cell> temp;
        for (int j = 0; j < numTile; j++)
            temp.push_back(Cell(tiles.size()));
        cells.push_back(temp);
    }
}

void Grid::generateMap()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        draw();
        if (!isCompeleteCollapsed())
            process(); // If compeletely collapsed then process algo
        try
        {
        }
        catch (...)
        {
            restart(); // If contradiction occured restart the algorithm
        }

        EndDrawing();
    }
}
