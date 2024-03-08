#include <iostream>
#include "AV_vector.h"



int main()
{
    AV_vector<int> vec(5,5);
    for (size_t i = 0; i < 10; i++)
    {
        vec.push_back(i);
        std::cout << vec[i];
    }

    return 0;
}