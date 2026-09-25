#include "rsg.h"

void ExpandSymbol(const std::string& symbol,
                  const Grammar& grammar,
                  RandomGenerator& random,
                  std::vector<std::string>& output) {
  // TODO: Recursively expand symbol and append terminal tokens to output.
  static_cast<void>(symbol);
  static_cast<void>(grammar);
  static_cast<void>(random);
  static_cast<void>(output);
  // TODO(gio): remove implementation
  const auto& k = grammar.at(symbol);
  const auto& p = k.getRandomProduction(random);
  for (const auto& token : p) {
    if (token[0] == '<' && token[token.size() - 1] == '>') {
      ExpandSymbol(token, grammar, random, output);
    } else {
      output.push_back(token);
    }
  }
}
