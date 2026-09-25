#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <fstream>
#include <map>
#include <string>

#include "definition.h"

using Grammar = std::map<std::string, Definition>;

Grammar ReadGrammar(std::ifstream& input);

#endif  // GRAMMAR_H_
