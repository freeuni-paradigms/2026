#ifndef RSG_H_
#define RSG_H_

#include <string>
#include <vector>

#include "grammar.h"
#include "random.h"

void ExpandSymbol(const std::string& symbol,
                  const Grammar& grammar,
                  RandomGenerator& random,
                  std::vector<std::string>& output);

#endif  // RSG_H_
