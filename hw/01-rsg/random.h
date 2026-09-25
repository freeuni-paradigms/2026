#ifndef __random__
#define __random__

#include <random>

/*
 * File: random.h
 * --------------
 * Provides a random number generator so
 * that pseudo-random numbers can be produced.
 */

class RandomGenerator {

 public:

  /**
   * Constructor: RandomGenerator
   * ----------------------------
   * Constructs a new RandomGenerator object.
   */

  RandomGenerator();
  explicit RandomGenerator(std::mt19937::result_type seed);

  /**
   * Method: getRandomInteger
   * ------------------------
   * Generates a seemingly random integer between the two specified
   * integers, inclusive.  All numbers in the range [low, high] are
   * equally likely outcomes.  If low and high are the same, then
   * that number is guaranteed to be returned.  If low is greater than
   * high, then getRandomInteger asserts and ends the program.
   *
   * @param the lowest number we'd like to be considered as a return value.
   * @param the highest number we'd like to be considered as a return value.
   * @return some number drawn uniformly from the range [low, high].
   */

  int getRandomInteger(int low, int high);

 private:
  std::mt19937 engine;
};

#endif // ! __random__
