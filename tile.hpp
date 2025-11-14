#pragma once
#include <raylib.h>
#include <iostream>
#include<vector>
class tile
{
    Texture2D tileImage;    // Image Object
    std::string sockets[4]; // Directions / Sockets

    void rotate(int angle); // Rotates tile to the give angle. e.g 90 degrees (vertical->horizontal)
public:
    tile(std::string imagePath, std::vector<std::string> sockets, int angle);
    ~tile();
    void draw(int X, int Y, int size); // Draw the image on (X,Y) position
};