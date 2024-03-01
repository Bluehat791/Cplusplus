//https://leetcode.com/problems/two-sum/description/
#include<iostream>
#include<vector>

using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> result;
        //int tmp = 0;
        for (size_t j = 0; j < nums.size(); j++)
        {
            for (size_t i = j+1; i < nums.size(); i++)
            {
                if (nums[i] + nums[j] == target)
                {
                    result.push_back(j);
                    result.push_back(i);
                    return result;
                }
            }
        }
        return result;
    }
};

int main()
{
    Solution sol;
    vector<int> tmp2 = { 3,3 };
    vector<int> tmp = sol.twoSum(tmp2, 6);
    for (auto i : tmp)
    {
        std::cout << i << " ";
    }
    std::cout << "\n";

    return 0;
}