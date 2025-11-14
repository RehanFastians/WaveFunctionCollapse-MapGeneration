#include <cell.hpp>
#include <vector>

Cell::Cell(int entropyValue = 0)
{
    for (int i = 0; i < entropyValue; i++)
        entropy.push_back(i);
}