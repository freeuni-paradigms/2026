#ifndef _PARADIGMS_HW_SIX_DEGREES_SIX_DEGREES_H_
#define _PARADIGMS_HW_SIX_DEGREES_SIX_DEGREES_H_

#include <string>

#include "imdb.h"
#include "path.h"

// Returns a shortest path containing at most six movie connections.
// If no such path exists, returns an empty path rooted at source.
Path FindShortestPath(const IMDB& db, const std::string& source,
                      const std::string& target);

#endif  // _PARADIGMS_HW_SIX_DEGREES_SIX_DEGREES_H_
