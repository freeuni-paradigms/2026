#ifndef _PARADIGMS_HW_RSG_GRAMMAR_H_
#define _PARADIGMS_HW_RSG_GRAMMAR_H_

#include <fstream>
#include <map>
#include <string>

#include "definition.h"

using Grammar = std::map<std::string, Definition>;

/**
 * Reads and parses grammar definition from the given input stream.
 */
Grammar ReadGrammar(std::ifstream& input);

#endif  // _PARADIGMS_HW_RSG_GRAMMAR_H_
