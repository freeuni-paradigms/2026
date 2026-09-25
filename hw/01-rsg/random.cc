#include "random.h"

#include <cassert>
#include <random>

/**
 * Constructor: RandomGenerator
 * ----------------------------
 * Initializes a random number generator with independently owned state.
 */

RandomGenerator::RandomGenerator()
    : engine(std::random_device{}()) {}

RandomGenerator::RandomGenerator(std::mt19937::result_type seed)
    : engine(seed) {}

/**
 * Method: getRandomInteger
 * ------------------------
 * Returns a seemingly random number between
 * the specified low and high, inclusive.  Based
 * on Eric Roberts' implementation from his
 * CS106A text.
 */

int RandomGenerator::getRandomInteger(int low, int high)
{
  assert(low <= high);
  const double percent =
      engine() / (static_cast<double>(engine.max()) + 1.0);
  assert(percent >= 0.0 && percent < 1.0);
  const int offset = static_cast<int>(percent * (high - low + 1));
  return low + offset;
}
