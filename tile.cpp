#include "tile.hpp"
#include <iostream>
#include <algorithm>

Tile::Tile(std::string imagePath, std::vector<std::string> sockets, int angle)
{
    // Assigning Sockets 0->Upper and proceed clockwise

    for (int i = 0; i < 4; i++)
        this->sockets[i] = sockets[i];

    // Rotate image object
    // 0 -> Nothing Changes
    // 1 -> 90 deg (clockwise)
    // 2 -> 180 deg (clockwise)
    // 3 -> 270 deg (clockwise)

    // Rotating socket if required

    rotate(angle);

    // Initiallize image object through image path

    Image tempImage = LoadImage(imagePath.c_str());

    // Rotating image object if needed

    for (int i = 0; i < angle; i++)
        ImageRotateCW(&tempImage);

    tileImage = LoadTextureFromImage(tempImage);

    UnloadImage(tempImage);
}

void Tile::rotate(int angle)
{

    std::string tempSockets[4];
    // Assign in temporary sockets to avoid loss of data in original sockets
    for (int i = 0; i < 4; i++)
    {
        tempSockets[(i + angle) % 4] = sockets[i];
    }
    // Store in the original sockets
    for (int i = 0; i < 4; i++)
    {
        sockets[i] = tempSockets[i];
    }
}

void Tile::draw(int y, int x, int tileSize)
{
    DrawTexturePro(
        tileImage,
        Rectangle{0, 0, (float)tileImage.width, (float)tileImage.height},                          // full texture
        Rectangle{(float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize}, // draw tileSize x tileSize size at (y, x)
        Vector2{0, 0},                                                                             // origin (no rotation)
        0.0f,                                                                                      // rotation angle
        WHITE);
}

std::string reverseString(std::string s)
{
    std::reverse(s.begin(), s.end());
    return s;
}

bool Tile::isPossible(Tile &other, int direction)
{

    // Direction 0: other tile is upward, 1: other tile is on the righ and so on....

    return this->sockets[direction] == reverseString(other.sockets[(direction + 2) % 4]);
}

Tile::~Tile()
{
    UnloadTexture(tileImage);
}