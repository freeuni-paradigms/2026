#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "rsg.h"

namespace {

void PrintVersion(int version, const std::vector<std::string>& output) {
  std::cout << "Version #" << version << ": ---------------------------\n";
  for (std::size_t i = 0; i < output.size(); ++i) {
    if (i != 0) {
      std::cout << ' ';
    }
    std::cout << output[i];
  }
  std::cout << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cerr << "You need to specify the name of a grammar file." << std::endl;
    std::cerr << "Usage: rsg <path to grammar text file>" << std::endl;
    return 1;
  }

  std::ifstream grammar_file(argv[1]);
  if (grammar_file.fail()) {
    std::cerr << "Failed to open the file named \"" << argv[1]
              << "\".  Check to ensure the file exists. " << std::endl;
    return 2;
  }

  const Grammar grammar = ReadGrammar(grammar_file);

  RandomGenerator random;
  for (int version = 1; version <= 3; ++version) {
    std::vector<std::string> output;
    ExpandSymbol("<start>", grammar, random, output);
    PrintVersion(version, output);
    if (version != 3) {
      std::cout << '\n';
    }
  }

  return 0;
}
