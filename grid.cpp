#include <iostream>
#include <tile.hpp>
#include <grid.hpp>
#include <vector>

grid::grid(int numTile)
{
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
        path += '0' + i;
        path += ".png";
        for (int angle = 0; angle < 4; angle++)
            tiles.push_back(tile(path, tempSockets[i], angle));
    }
}