//https://leetcode.com/problems/longest-common-prefix/description/
#include<iostream>
#include<string>
#include<vector>

using namespace std;
class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if (strs[0] == "")
            return "";
        string result = strs[0];
        for (size_t i = 1; i < strs.size(); i++)
        {
            int j = 0;
            while (result[j] == strs[i][j] && j < strs[i].size())
            {
                j++;
            }
            result.resize(j);
        }
        //result.shrink_to_fit();
        return result;
    }
};

int main()
{
    Solution sol;
    vector<string> tmp{ "flower","flower","flower","flower" };
    std::cout << sol.longestCommonPrefix(tmp) << "\n";
    return 0;
}