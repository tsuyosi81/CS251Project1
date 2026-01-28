#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;

vector<int> mystery(const vector<int>& vec) {
  set<int> myset;
  vector<int> ret;
  for (int str : vec) {
    auto ins = myset.insert(str);
    // if successfully inserted, ins.second is true
    if (ins.second) {
      ret.push_back(str);
    }
  }
  return ret;
}

// input is {2, 5, 2, -1, 5}. What does mystery do? Give it a documentation
// comment or a better name.

int main() {
  vector<int> vec = {2, 5, 2, -1, 5};
  vector<int> result = mystery(vec);
  for (int val : result) {
    cout << val << " ";
  }
}