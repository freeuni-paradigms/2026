#include "grammar.h"

Grammar ReadGrammar(std::ifstream& input) {
  Grammar grammar;
  while (true) {
    std::string ignored;
    std::getline(input, ignored, '{');
    if (input.eof()) {
      return grammar;
    }
    input.putback('{');
    Definition definition(input);
    grammar[definition.getNonterminal()] = definition;
  }
}
