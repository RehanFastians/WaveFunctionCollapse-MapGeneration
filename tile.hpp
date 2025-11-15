#pragma once
#include <raylib.h>
#include <iostream>
#include <vector>
class Grid;
class Tile
{
    Texture2D tileImage;    // Image Object
    std::string sockets[4]; // Directions / Sockets

    void rotate(int angle); // Rotates tile to the give angle.
public:
    Tile(std::string imagePath, std::vector<std::string> sockets, int angle);
    void draw(int y, int x, int tileSize);       // Draw the image on (X,Y) position
    bool isPossible(Tile &other, int direction); // Direction specify Tile other is in which direction

    Tile(const Tile &) = delete; // disable copy
    Tile &operator=(const Tile &) = delete;

    Tile(Tile &&) = default; // allow move
    Tile &operator=(Tile &&) = default;
    ~Tile();

    friend class Grid;
};