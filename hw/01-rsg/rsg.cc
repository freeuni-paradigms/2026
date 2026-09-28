#include "rsg.h"

using namespace std;

void ExpandSymbol(const string& symbol,
                  const Grammar& grammar,
                  RandomGenerator& random,
                  vector<string>& output) {
  // TODO: Implement
  const auto& k = grammar.find(symbol);
  if (k == grammar.end()) {
    output.push_back(symbol);
  } else {
    for (const auto& token : k->second.getRandomProduction(random)) {
      ExpandSymbol(token, grammar, random, output);
    }
  }
}
