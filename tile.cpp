#include <tile.hpp>
#include <iostream>

tile::tile(std::string imagePath, std::string sockets[4], int angle)
{
    // Assigning Sockets

    this->sockets[0] = sockets[0];
    this->sockets[1] = sockets[1];
    this->sockets[2] = sockets[2];
    this->sockets[3] = sockets[3];

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

void tile::rotate(int angle = 0)
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

void tile::draw(int X, int Y, int size)
{
    // Raylib Work
}

tile::~tile()
{
    UnloadTexture(tileImage);
}