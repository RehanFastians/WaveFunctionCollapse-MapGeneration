#include "grid.hpp"
#include <string>

Grid::Grid(int numTile)
{
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

    const int tileSize = gridDim / numTile;
    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() == 0)
                throw("Contradiction encountered in Wave Function Collapse");
            if (cells[y][x].entropy.size() == 1)
                tiles[cells[y][x].entropy[0]].draw(y, x, tileSize);
        }
    }
}

std::pair<int, int> Grid ::findLeastEntropy()
{

    // Find the first occcurence of the least entropy value cell

    int leastEntropy = tiles.size();
    std ::pair<int, int> cellPosition = {0, 0};
    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() < leastEntropy && cells[y][x].entropy.size() != 1)
            {
                leastEntropy = cells[y][x].entropy.size();
                cellPosition = {y, x};
            }
        }
    }
    return cellPosition;
}

bool Grid::isCompeleteCollapsed()
{

    // This function checks if the map is compeletely collapse or not

    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() != 1)
                return false;
        }
    }
    return true;
}

void Grid::processCell(int y, int x, std::queue<std::pair<int, int>> &bfs)
{

    // These directions idx are in congruence with the directions index in tiles checking function

    int diry[] = {-1, 0, 1, 0};
    int dirx[] = {0, 1, 0, -1};

    for (int direction = 0; direction < 4; direction++)
    {
        int nextX = x + dirx[direction];
        int nextY = y + diry[direction];
        if (nextX < 0 || nextY < 0 || nextX >= numTile || nextY >= numTile)
            continue;
        bool toPush = false;
        for (int j = 0; j < cells[nextY][nextX].entropy.size(); j++)
        {
            bool anyCollapsed = true;
            for (int i = 0; i < cells[y][x].entropy.size(); i++)
            {
                if (tiles[cells[nextY][nextX].entropy[j]].isPossible(tiles[cells[y][x].entropy[i]], direction))
                {
                    anyCollapsed = false;
                    break;
                }
                // anyCollapsed = true;
            }
            if (anyCollapsed)
            {
                toPush = true;
                cells[nextY][nextX].collapse(j);
                j--;
            }
        }
        if (toPush)
            bfs.push({nextY, nextX});
    }
}

void Grid::process()
{

    // queue to store the next cell whose entropy is to collapse

    std::queue<std::pair<int, int>> bfs;

    // Data structure to store info of the visited cells to improve time complexity

    std::vector<std::vector<bool>> visited(numTile, std::vector<bool>(numTile, false));

    // Finding least Entropy and will collapse it
    std ::pair<int, int> startCell = findLeastEntropy();

    // Randomly collapsing the cell

    cells[startCell.first][startCell.second].randomCollapse();

    bfs.push(startCell);

    while (!bfs.empty())
    {
        auto [y, x] = bfs.front();
        bfs.pop();
        visited[y][x] = true;

        processCell(y, x, bfs);
    }
};

void Grid::generateMap()
{
    try
    {

        while (!WindowShouldClose())
        {
            BeginDrawing();

            ClearBackground(BLACK);

            if (!isCompeleteCollapsed())
                process();

            draw();

            EndDrawing();
        }
    }
    catch (...)
    {
        CloseWindow();
    }
}