#include<iostream>
#include<vector>
#include <algorithm>
using namespace std;
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
       vector<vector<int>> result;
       int n = nums.size();
       if (n < 3) return result;
       sort(nums.begin(), nums.end());
       vector<int>::iterator p1, p2, p3;

       for (p1 = nums.begin(); p1 != nums.end() - 2; ++p1) {
           if (p1 != nums.begin() && *p1 == *(p1 - 1)) {
               continue;
           }
           p2 = p1 + 1;   
           p3 = nums.end() - 1; 

           while (p2 < p3) {
               int sum = *p1 + *p2 + *p3;

               if (sum == 0) {
                   result.push_back({ *p1, *p2, *p3 });
                   while (p2 < p3 && *p2 == *(p2 + 1)) {
                       ++p2;
                   }
                   while (p2 < p3 && *p3 == *(p3 - 1)) {
                       --p3;
                   }
                   ++p2;
                   --p3;
               }
               else if (sum < 0) {
                   ++p2;
               }
               else {
                   --p3;
               }
           }
       }
       return result;
    }
};
// 测试函数
void printResult(const vector<vector<int>>& result) {
    for (const auto& triplet : result) {
        cout << "[";
        for (size_t i = 0; i < triplet.size(); ++i) {
            cout << triplet[i];
            if (i < triplet.size() - 1) {
                cout << ", ";
            }
        }
        cout << "] ";
    }
    cout << endl;
}
void testThreeSum() {
    Solution solution;

    // 测试用例1：包含多个有效三元组
    vector<int> nums1 = { -1, 0, 1, 2, -1, -4 };
    cout << "测试用例1: [-1, 0, 1, 2, -1, -4]" << endl;
    cout << "预期结果: [-1, -1, 2] [-1, 0, 1] " << endl;
    cout << "实际结果: ";
    printResult(solution.threeSum(nums1));
    cout << endl;

    // 测试用例2：无有效三元组
    vector<int> nums2 = { 0, 1, 1 };
    cout << "测试用例2: [0, 1, 1]" << endl;
    cout << "预期结果: " << endl;
    cout << "实际结果: ";
    printResult(solution.threeSum(nums2));
    cout << endl;

    // 测试用例3：全为0的情况
    vector<int> nums3 = { 0, 0, 0, 0 };
    cout << "测试用例3: [0, 0, 0, 0]" << endl;
    cout << "预期结果: [0, 0, 0] " << endl;
    cout << "实际结果: ";
    printResult(solution.threeSum(nums3));
    cout << endl;

    // 测试用例4：空输入
    vector<int> nums4;
    cout << "测试用例4: 空数组" << endl;
    cout << "预期结果: " << endl;
    cout << "实际结果: ";
    printResult(solution.threeSum(nums4));
    cout << endl;

    // 测试用例5：元素数量不足3个
    vector<int> nums5 = { 1, 2 };
    cout << "测试用例5: [1, 2]" << endl;
    cout << "预期结果: " << endl;
    cout << "实际结果: ";
    printResult(solution.threeSum(nums5));
    cout << endl;
}

int main() {
    testThreeSum();
    return 0;
}
