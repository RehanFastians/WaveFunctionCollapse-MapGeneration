#pragma once
#include<vector>

class Grid;
class Cell{
    std :: vector<int> entropy;
    
    public:
    Cell(int entropyValue);
    void collapse(int index);
    void randomCollapse();
    friend class Grid;
};