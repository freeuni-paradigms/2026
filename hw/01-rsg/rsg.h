#ifndef _PARADIGMS_HW_RSG_RSG_H_
#define _PARADIGMS_HW_RSG_RSG_H_

#include <string>
#include <vector>

#include "grammar.h"
#include "random.h"

using namespace std;

void ExpandSymbol(const string& symbol,
                  const Grammar& grammar,
                  RandomGenerator& random,
                  vector<string>& output);

#endif  // _PARADIGMS_HW_RSG_RSG_H_
