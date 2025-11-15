#include "cell.hpp"
#include<cstdlib>

Cell::Cell(int entropyValue = 0)
{
    for (int i = 0; i < entropyValue; i++)
        entropy.push_back(i);
}
void Cell::collapse(int index)
{

    // This function will collapse the entropy at index

    entropy.erase(entropy.begin() + index);
}

void Cell::randomCollapse(){

    // This function will randomly select an element and collapse all the others

    int randomPick = rand()%entropy.size();

    entropy = {entropy[randomPick]};

}