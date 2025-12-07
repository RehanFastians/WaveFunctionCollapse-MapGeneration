#include "grid.hpp"
#include "stack.hpp"
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <nlohmann/json.hpp>
#include "minHeap.hpp"
#include <functional>
#include <utility>
#include <vector>
using json = nlohmann::json;

// ----------------------------------------
//      CONSTRUCTOR AND INITIALIZER
// ----------------------------------------

Grid::Grid(int numTile)
{
    // Seeding rand() function

    srand(time(0));

    //  Initializing window for showing generated map

    InitWindow((float)(gridDim + 40), (float)(gridDim + 90), "Wave-Function-Collapse");

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

// -----------------------------------------
//           HELPER FUNCTIONS
// -----------------------------------------

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
    entropyMinHeap = MinHeap<std::pair<int, int>>();
    entropyMinHeap.push({tiles.size(), 0});
}

bool Grid::isCompeleteCollapsed()
{

    // This function checks if the map is compeletely collapse or not
    return numTile * numTile == collapsedCount;
}

// --------------------------------------
//        MAIN GENERATION FLOW
// --------------------------------------

void Grid::generateMap()
{
    SetTargetFPS(120);
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
            try
            {
                if ((!isPaused) && (!isCompeleteCollapsed()))
                    process();
            }
            catch (...)
            {
                restart(); // If contradiction occured restart the algorithm
            }
        }
        EndDrawing();
    }
}

// ----------------------------------------
//         ALGORITHM LOGIC SIMULATION
// ----------------------------------------

void Grid::process()
{
    // Fetch the least entropy value cell from the minHeap

    auto [entropy, index] = entropyMinHeap.top();
    entropyMinHeap.pop();
    int y = index / numTile, x = index % numTile;
    if (entropy > cells[y][x].entropy.size())
        return;
    cells[y][x].randomCollapse();

    // Process that cell
    propagate(x, y);
    // processCell(x, y);
}


void Grid::propagate(int startX, int startY)
{
    Stack<std::pair<int, int>> stack;
    stack.push({startX, startY});

    while (!stack.empty())
    {
        auto [cx, cy] = stack.top();
        stack.pop();

        // Check all 4 neighbors
        int diry[] = {-1, 0, 1, 0}; // Up, Right, Down, Left
        int dirx[] = {0, 1, 0, -1};

        // Note: Make sure you handle opposite directions correctly 
        // If checking UP (dir 0), the neighbor looks DOWN (dir 2)
        int oppositeDir[] = {2, 3, 0, 1}; 

        for (int d = 0; d < 4; d++)
        {
            int nx = cx + dirx[d];
            int ny = cy + diry[d];

            // Bounds check
            if (nx < 0 || ny < 0 || nx >= numTile || ny >= numTile) continue;

            // Check if we need to remove options from the Neighbor (nx, ny)
            // based on the valid options remaining in Current (cx, cy)
            
            bool neighborChanged = false;
            std::vector<int>& nOptions = cells[ny][nx].entropy;
            
            // Iterate backwards so we can erase efficiently
            for (int i = nOptions.size() - 1; i >= 0; i--)
            {
                int neighborTileID = nOptions[i];
                bool isCompatibleWithAny = false;

                // Look at every option still possible in the CURRENT cell
                for (int myTileID : cells[cy][cx].entropy)
                {
                    // Does MyTile allow NeighborTile in direction 'd'?
                    if (tiles[myTileID].isPossible(tiles[neighborTileID], d)) {
                        isCompatibleWithAny = true;
                        break;
                    }
                }

                // If this neighbor option isn't compatible with ANY of my options, remove it
                if (!isCompatibleWithAny)
                {
                    cells[ny][nx].collapse(i); // Remove option at index i
                    neighborChanged = true;
                }
            }

            // CRITICAL: If the neighbor changed, we must check *its* neighbors too!
            if (neighborChanged)
            {
                if (cells[ny][nx].entropy.empty()) {
                    throw std::runtime_error("Contradiction"); // Caught by your main loop
                }
                
                // Add to stack to propagate further
                stack.push({nx, ny});
                
                // Update heap (Optional: creates duplicate entries but is safer)
                entropyMinHeap.push({(int)cells[ny][nx].entropy.size(), ny * numTile + nx});
            }
        }
    }
}

void Grid ::processCell(int x, int y)
{

    // These directions indices are in congruence with the directions index in tiles checking function

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
                //  If its not compatible with any neighbour then collapse it

                cells[nextY][nextX].collapse(i);
                if (cells[nextY][nextX].entropy.size() < 1)
                {
                    restart(); // Restart if entropy gets reduced to 0
                    return;
                }
                entropyMinHeap.push({cells[nextY][nextX].entropy.size(), nextY * numTile + nextX});
                i--;
            }
        }
    }
}

// -----------------------------------
//       ALL DRAWING FUNCTIONS
// -----------------------------------

void Grid::showHomeScreen()
{
    // 1. Setup Dimensions & Layout

    // Drawing a subtle "Graph Paper" background pattern

    for (int i = 0; i < gridDim + 80; i += 40)
    {
        DrawLine(i, 0, i, gridDim + 80, Fade(GRAY, 0.4f));
        DrawLine(0, i, gridDim + 80, i, Fade(GRAY, 0.4f));
    }

    // Button settings

    int btnWidth = 200;
    int btnHeight = 60;
    Rectangle btnBounds = {
        (float)((gridDim - btnWidth) / 2),
        (float)((gridDim + 50) / 2),
        (float)btnWidth,
        (float)btnHeight};

    // 2. Handle Input

    Vector2 mousePoint = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mousePoint, btnBounds);

    if (isHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isHomeScreen = false;
        return;
    }

    // 3. Drawing

    // A. Draw TITLE "WFC"

    const char *titleText = "WFC";
    int titleFontSize = 120;
    int titleWidth = MeasureText(titleText, titleFontSize);

    // Positions

    int titleX = (gridDim - titleWidth) / 2;
    int titleY = (gridDim - 250) / 2;

    // Soft Drop Shadow

    DrawText(titleText, titleX + 6, titleY + 6, titleFontSize, Fade(BLACK, 0.2f));

    // Main Text in Midnight Blue

    DrawText(titleText, titleX, titleY, titleFontSize, (Color){20, 30, 60, 255});

    // B. Draw Start Button

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
        // Normal State: White with Dark Blue Border
        DrawRectangleRounded(btnBounds, roundness, segments, WHITE);
        DrawRectangleRoundedLines(btnBounds, roundness, segments, (Color){40, 60, 140, 255});
    }

    // C. Draw "START" Text inside Button

    const char *btnText = "START";
    int btnFontSize = 30;
    int btnTextWidth = MeasureText(btnText, btnFontSize);

    int textX = (int)btnBounds.x + (btnWidth - btnTextWidth) / 2;
    int textY = (int)btnBounds.y + (btnHeight - btnFontSize) / 2;

    // Text color flips based on hover

    Color textColor = isHovering ? WHITE : (Color){40, 60, 140, 255};
    DrawText(btnText, textX, textY, btnFontSize, textColor);

    // D. Subtitle

    const char *subText = "Wave Function Collapse";
    int subSize = 20;
    int subWidth = MeasureText(subText, subSize);

    // Letter spacing effect (drawing text slightly darker)

    DrawText(subText, (gridDim - subWidth) / 2, (gridDim - 40) / 2, subSize, (Color){80, 80, 100, 255});
}

void Grid::draw()
{
    // Define Theme Colors
    Color themeDarkBlue = {20, 30, 60, 255}; // Main accent color
    Color themePause = {39, 174, 96, 255};   // Emerald Green
    Color themeRestart = {192, 57, 43, 255}; // Alizarin Red
    Color themeHome = {142, 68, 173, 255};   // Amethyst Purple

    // Background & Grid Pattern

    for (int i = 0; i < gridDim + 80; i += 40)
    {
        DrawLine(i, 0, i, gridDim + 80, Fade(GRAY, 0.3f));
        DrawLine(0, i, gridDim + 80, i, Fade(GRAY, 0.3f));
    }

    const int tileSize = gridDim / numTile;

    // NEW: Main Grid Border
    // Draws a professional-looking border around the tile area

    Rectangle gridBorderRect = {20, 10, (float)gridDim, (float)gridDim};

    // We loop 4 times to create a 4px thick line (thickness only supported in newer version of raylib so have to do it mannually)

    for (int i = 0; i < 4; i++)
    {
        Rectangle currentRect = {
            gridBorderRect.x - i,           // Move left
            gridBorderRect.y - i,           // Move up
            gridBorderRect.width + (i * 2), // Grow width
            gridBorderRect.height + (i * 2) // Grow height
        };

        DrawRectangleRoundedLines(currentRect, 0.01f, 10, themeDarkBlue);
    }

    // 1. Pattern Drawing Logic

    collapsedCount = 0;

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

    // 2. UI Layout

    float btnWidth = 140;
    float btnHeight = 45;
    float padding = 20;
    float bottomMargin = 10;

    float centerX = (float)(gridDim + 80) / 2;
    float btnY = (float)(gridDim + 85) - btnHeight - bottomMargin;

    float totalWidth = (btnWidth * 3) + (padding * 2);
    float startX = centerX - (totalWidth / 2);

    Rectangle pauseRec = {startX, btnY, btnWidth, btnHeight};
    Rectangle restartRec = {startX + btnWidth + padding, btnY, btnWidth, btnHeight};
    Rectangle homeRec = {startX + (btnWidth + padding) * 2, btnY, btnWidth, btnHeight};

    Vector2 mousePoint = GetMousePosition();
    bool pauseHover = CheckCollisionPointRec(mousePoint, pauseRec);
    bool restartHover = CheckCollisionPointRec(mousePoint, restartRec);
    bool homeHover = CheckCollisionPointRec(mousePoint, homeRec);

    // 3. Drawing Buttons

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

    if (pauseHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
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

    if (homeHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isHomeScreen = true;
        restart();
        isPaused = false;
        return;
    }
}