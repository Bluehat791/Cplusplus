#include<iostream>
#include "AV_List.h"

int main()
{
    List<int> lst;

    for (size_t i = 0; i < 20; ++i)
    {
        lst.push_back(i);
    }

    for (size_t i = 0; i < 20; ++i)
    {
        std::cout << lst[i] << "\n";
    }
    
    
    return 0;
}