#include <iostream>
#include <unordered_map>
#include <vector>

int main(){
  std::unordered_map<int, int> t;
  t[0] = 2;
  t[3123] = 5555;
  for(auto i : t){
    std::cout << i.first << std::endl;
  }
  return 0;
}
