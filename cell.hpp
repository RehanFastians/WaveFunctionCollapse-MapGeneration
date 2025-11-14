#pragma once
#include<iostream>
#include<vector>
class Grid;
class Cell{
    std :: vector<int> entropy;
    
    public:
    Cell(int entropyValue);
    friend class Grid;
};