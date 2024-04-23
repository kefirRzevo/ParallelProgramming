#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::ifstream t("exp.txt");
  std::stringstream buffer;
  buffer << t.rdbuf();
  std::string in_str = buffer.str(), out_str;
  for (auto it = 0; it < in_str.size(); ++it) {
    if (in_str[it] != ' ') {
      out_str.push_back(in_str[it]);
    }
  }
  std::ofstream out{"exp_out.txt"};
  out << out_str;
}
