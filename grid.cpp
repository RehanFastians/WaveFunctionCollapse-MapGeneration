#include "grid.hpp"
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <nlohmann/json.hpp>
#include <queue>
#include <functional>
#include <utility>
#include <vector>
using json = nlohmann::json;

Grid::Grid(int numTile)
{
    // Seeding rand() function

    srand(time(0));

    //  Initializing window for showing generated map

    InitWindow((float)(gridDim + 40), (float)(gridDim + 80), "Wave-Function-Collapse");

    this->numTile = numTile;

    loadSockets("circuit");

    // Intitalizing default entropy to the cells

    for (int i = 0; i < numTile; i++)
    {
        std ::vector<Cell> temp;
        for (int j = 0; j < numTile; j++)
            temp.push_back(Cell(tiles.size()));
        cells.push_back(temp);
    }
    entropyMinHeap.push({tiles.size(), 0});
}

void Grid ::loadSockets(std ::string type)
{

    // Fetching sockets from sockets.json

    std::ifstream in("sockets.json");
    json data;
    in >> data;

    std::vector<std::vector<std::string>> tempSockets;

    for (auto &arr : data[type])
    {
        std::vector<std::string> sockets;

        for (auto &s : arr)
            sockets.push_back(s.get<std::string>());

        tempSockets.push_back(sockets);
    }

    // Setting tiles

    tiles.reserve(tempSockets.size() * 4);
    for (int i = 0; i < tempSockets.size(); i++)
    {
        std::string path = "Images/";
        path += type;
        path += '/';
        path += std::to_string(i);
        path += ".png";
        for (int angle = 0; angle < 4; angle++)
            tiles.emplace_back(path, tempSockets[i], angle);
    }
}

void Grid::draw()
{
    // Define Theme Colors (Matching the Home Screen vibe)
    Color themeDarkBlue = {20, 30, 60, 255}; // Main accent color
    Color themePause = {39, 174, 96, 255};   // Emerald Green
    Color themeRestart = {192, 57, 43, 255}; // Alizarin Red
    Color themeHome = {142, 68, 173, 255};   // Amethyst Purple

    // ---------------------------------------------------------
    // Background & Grid Pattern (Kept from your code)
    // ---------------------------------------------------------
    for (int i = 0; i < gridDim + 80; i += 40)
    {
        DrawLine(i, 0, i, gridDim + 80, Fade(GRAY, 0.3f)); // Slightly subtler gray
        DrawLine(0, i, gridDim + 80, i, Fade(GRAY, 0.3f));
    }

    // ---------------------------------------------------------
    // NEW: Main Grid Border
    // ---------------------------------------------------------
    // Draws a professional-looking border around the tile area
    Rectangle gridBorderRect = {-4, -4, (float)gridDim + 8, (float)gridDim + 8};
    DrawRectangleRoundedLines(gridBorderRect, 0.01f, 10, themeDarkBlue);

    // ---------------------------------------------------------
    // 1. Existing Grid Drawing Logic (UNMODIFIED)
    // ---------------------------------------------------------
    collapsedCount = 0;
    const int tileSize = gridDim / numTile;

    for (int y = 0; y < numTile; y++)
    {
        for (int x = 0; x < numTile; x++)
        {
            if (cells[y][x].entropy.size() == 0)
                throw("Contradiction encountered in Wave Function Collapse");

            if (cells[y][x].entropy.size() == 1)
            {
                tiles[cells[y][x].entropy[0]].draw(y, x, tileSize);
                collapsedCount++;
            }
        }
    }

    // ---------------------------------------------------------
    // 2. Modern UI Layout (UNMODIFIED POSITIONS)
    // ---------------------------------------------------------
    float btnWidth = 140;
    float btnHeight = 45;
    float padding = 20;
    float bottomMargin = 10;

    float centerX = (float)(gridDim + 80) / 2;
    float btnY = (float)(gridDim + 80) - btnHeight - bottomMargin;

    float totalWidth = (btnWidth * 3) + (padding * 2);
    float startX = centerX - (totalWidth / 2);

    Rectangle pauseRec = {startX, btnY, btnWidth, btnHeight};
    Rectangle restartRec = {startX + btnWidth + padding, btnY, btnWidth, btnHeight};
    Rectangle homeRec = {startX + (btnWidth + padding) * 2, btnY, btnWidth, btnHeight};

    Vector2 mousePoint = GetMousePosition();
    bool pauseHover = CheckCollisionPointRec(mousePoint, pauseRec);
    bool restartHover = CheckCollisionPointRec(mousePoint, restartRec);
    bool homeHover = CheckCollisionPointRec(mousePoint, homeRec);

    // ---------------------------------------------------------
    // 3. Drawing Buttons (THEMED TO MATCH HOME SCREEN)
    // ---------------------------------------------------------

    // Common button settings
    float roundness = 0.5f; // Pill shape
    int segments = 10;
    int fontSize = 20;


    // A. Themed UI Panel Background
    // Replaced the dark glass with a clean white panel with a dark blue border
    Rectangle panelRect = {startX - 15, btnY - 10, totalWidth + 30, btnHeight + 20};
    DrawRectangleRounded(panelRect, 0.2f, 10, Fade(BLACK, 0.6f));
    DrawRectangleRoundedLines(panelRect, 0.2f, 10, themeDarkBlue);

    // --- PAUSE BUTTON ---
    Color currentPauseColor = pauseHover ? themePause : WHITE;
    Color currentPauseBorder = pauseHover ? themeDarkBlue : themePause;
    Color currentPauseText = pauseHover ? WHITE : themeDarkBlue;
    const char *pauseLabel = isPaused ? "RESUME" : "PAUSE";

    // Shadow
    DrawRectangleRounded({pauseRec.x + 3, pauseRec.y + 3, pauseRec.width, pauseRec.height}, roundness, segments, Fade(BLACK, 0.1f));
    // Body & Border
    DrawRectangleRounded(pauseRec, roundness, segments, currentPauseColor);
    DrawRectangleRoundedLines(pauseRec, roundness, segments, currentPauseBorder);
    // Text
    DrawText(pauseLabel, (int)(pauseRec.x + (btnWidth - MeasureText(pauseLabel, fontSize)) / 2), (int)(pauseRec.y + (btnHeight - fontSize) / 2), fontSize, currentPauseText);

    if (pauseHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        isPaused = !isPaused;
        return;
    }

    // --- RESTART BUTTON ---
    Color currentRestartColor = restartHover ? themeRestart : WHITE;
    Color currentRestartBorder = restartHover ? themeDarkBlue : themeRestart;
    Color currentRestartText = restartHover ? WHITE : themeDarkBlue;

    // Shadow
    DrawRectangleRounded({restartRec.x + 3, restartRec.y + 3, restartRec.width, restartRec.height}, roundness, segments, Fade(BLACK, 0.1f));
    // Body & Border
    DrawRectangleRounded(restartRec, roundness, segments, currentRestartColor);
    DrawRectangleRoundedLines(restartRec, roundness, segments, currentRestartBorder);
    // Text
    DrawText("RESTART", (int)(restartRec.x + (btnWidth - MeasureText("RESTART", fontSize)) / 2), (int)(restartRec.y + (btnHeight - fontSize) / 2), fontSize, currentRestartText);

    if (restartHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        restart();
        isPaused = false;
        return;
    }

    // --- HOME BUTTON ---
    Color currentHomeColor = homeHover ? themeHome : WHITE;
    Color currentHomeBorder = homeHover ? themeDarkBlue : themeHome;
    Color currentHomeText = homeHover ? WHITE : themeDarkBlue;

    // Shadow
    DrawRectangleRounded({homeRec.x + 3, homeRec.y + 3, homeRec.width, homeRec.height}, roundness, segments, Fade(BLACK, 0.1f));
    // Body & Border
    DrawRectangleRounded(homeRec, roundness, segments, currentHomeColor);
    DrawRectangleRoundedLines(homeRec, roundness, segments, currentHomeBorder);
    // Text
    DrawText("HOME", (int)(homeRec.x + (btnWidth - MeasureText("HOME", fontSize)) / 2), (int)(homeRec.y + (btnHeight - fontSize) / 2), fontSize, currentHomeText);

    if (homeHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        isHomeScreen = true;
        restart();
        isPaused = false;
        return;
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
                leastEntropy = cells[y][x].entropy.size();
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
    entropyMinHeap = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>>();
    entropyMinHeap.push({tiles.size(), 0});
}

void Grid::showHomeScreen()
{
    // ----------------------------
    // 1. Setup Dimensions & Layout (UNMODIFIED POSITIONS)
    // ----------------------------


    // Draw a subtle "Graph Paper" background pattern
    // This fills the empty space and fits the "Grid" theme perfectly
    for (int i = 0; i < gridDim + 80; i += 40)
    {
        DrawLine(i, 0, i, gridDim + 80, Fade(GRAY, 0.4f));
        DrawLine(0, i, gridDim + 80, i, Fade(GRAY, 0.4f));
    }

    // Button settings (KEPT SAME)
    int btnWidth = 200;
    int btnHeight = 60;
    Rectangle btnBounds = {
        (float)((gridDim - btnWidth) / 2),
        (float)((gridDim + 50) / 2),
        (float)btnWidth,
        (float)btnHeight};

    // ----------------------------
    // 2. Handle Input (KEPT SAME)
    // ----------------------------
    Vector2 mousePoint = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mousePoint, btnBounds);

    if (isHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isHomeScreen = false;
        return;
    }

    // ----------------------------
    // 3. Drawing (VISUAL UPGRADE)
    // ----------------------------

    // A. Draw Big Fancy Title "WFC"
    const char *titleText = "WFC";
    int titleFontSize = 120;
    int titleWidth = MeasureText(titleText, titleFontSize);

    // Positions (KEPT SAME)
    int titleX = (gridDim - titleWidth) / 2;
    int titleY = (gridDim - 250) / 2;

    // UPGRADE: Soft Drop Shadow (looks cleaner than black)
    DrawText(titleText, titleX + 6, titleY + 6, titleFontSize, Fade(BLACK, 0.2f));
    // UPGRADE: Main Text in "Midnight Blue" instead of Maroon
    DrawText(titleText, titleX, titleY, titleFontSize, (Color){20, 30, 60, 255});

    // B. Draw Start Button
    // UPGRADE: Use Rounded Rectangles (Pill shape)
    float roundness = 0.5f;
    int segments = 10;

    // Draw Button Shadow
    Rectangle shadowBounds = btnBounds;
    shadowBounds.y += 4; // Slight offset down
    DrawRectangleRounded(shadowBounds, roundness, segments, Fade(BLACK, 0.15f));

    // Draw Button Body
    if (isHovering)
    {
        // Hover State: Filled Dark Blue with White Text
        DrawRectangleRounded(btnBounds, roundness, segments, (Color){40, 60, 140, 255});
        DrawRectangleRoundedLines(btnBounds, roundness, segments, (Color){20, 30, 60, 255});
    }
    else
    {
        // Normal State: White with Dark Blue Border (Clean look)
        DrawRectangleRounded(btnBounds, roundness, segments, WHITE);
        DrawRectangleRoundedLines(btnBounds, roundness, segments, (Color){40, 60, 140, 255});
    }

    // C. Draw "START" Text inside Button
    const char *btnText = "START";
    int btnFontSize = 30;
    int btnTextWidth = MeasureText(btnText, btnFontSize);

    int textX = (int)btnBounds.x + (btnWidth - btnTextWidth) / 2;
    int textY = (int)btnBounds.y + (btnHeight - btnFontSize) / 2;

    // UPGRADE: Text color flips based on hover
    Color textColor = isHovering ? WHITE : (Color){40, 60, 140, 255};
    DrawText(btnText, textX, textY, btnFontSize, textColor);

    // D. Subtitle
    const char *subText = "Wave Function Collapse";
    int subSize = 20;
    int subWidth = MeasureText(subText, subSize);

    // UPGRADE: Letter spacing effect (drawing text slightly darker)
    DrawText(subText, (gridDim - subWidth) / 2, (gridDim - 40) / 2, subSize, (Color){80, 80, 100, 255});
}

void Grid::generateMap()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (isHomeScreen)
        {
            showHomeScreen();
        }
        else
        {
            draw();
            // if (!isCompeleteCollapsed())
            // process(); // If compeletely collapsed then process algo
            try
            {
                if (!isPaused)
                    processtemp();
            }
            catch (...)
            {
                restart(); // If contradiction occured restart the algorithm
            }
        }
        EndDrawing();
    }
}

void Grid::processtemp()
{
    auto [entropy, index] = entropyMinHeap.top();
    entropyMinHeap.pop();
    int y = index / numTile, x = index % numTile;
    if (entropy > cells[y][x].entropy.size())
        return;
    cells[y][x].randomCollapse();

    processCellTemp(x, y);
}

void Grid ::processCellTemp(int x, int y)
{

    // These directions idx are in congruence with the directions index in tiles checking function

    int diry[] = {-1, 0, 1, 0};
    int dirx[] = {0, 1, 0, -1};

    int currentEntropy = cells[y][x].entropy[0];

    for (int direction = 0; direction < 4; direction++)
    {
        int nextX = x + dirx[direction];
        int nextY = y + diry[direction];

        if (nextX < 0 || nextY < 0 || nextX >= numTile || nextY >= numTile) // Validating position
            continue;
        for (int i = 0; i < cells[nextY][nextX].entropy.size(); i++)
        {
            if (!tiles[currentEntropy].isPossible(tiles[cells[nextY][nextX].entropy[i]], direction))
            {
                cells[nextY][nextX].collapse(i);
                if (cells[nextY][nextX].entropy.size() < 1)
                {
                    restart();
                    return;
                }
                entropyMinHeap.push({cells[nextY][nextX].entropy.size(), nextY * numTile + nextX});
                i--;
            }
        }
    }
}