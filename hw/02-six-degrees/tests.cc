#include "imdb.h"
#include "six-degrees.h"

#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include "gl/lunit/lunit.h"
}

namespace {

bool PathEq(const Path& actual, const Path& expected) {
  return actual == expected;
}

void PathEqMsg(char* message, const Path& actual, const Path& expected) {
  std::ostringstream expectedOutput;
  std::ostringstream actualOutput;
  expectedOutput << expected;
  actualOutput << actual;
  std::snprintf(message, ERROR_MAX_LEN,
                "Expected path:\n%sActual path:\n%s",
                expectedOutput.str().c_str(), actualOutput.str().c_str());
}

#define ASSERT_PATH_EQ(actual, expected) \
  __ASSERT_EQ(PathEq, PathEqMsg, actual, expected)

struct Link {
  const char* title;
  int year;
  const char* player;
};

Path ExpectedPath(const std::string& source,
                  std::initializer_list<Link> links) {
  Path result(source);
  for (const Link& link : links) {
    result.addConnection({link.title, link.year}, link.player);
  }
  return result;
}

bool FileExists(const std::string& path) {
  std::ifstream input(path, std::ios::binary);
  return input.good();
}

std::string FindDataDirectory() {
  const char* candidates[] = {
      "data/little-endian",
      "../data/little-endian",
  };
  for (const char* candidate : candidates) {
    const std::string directory(candidate);
    if (FileExists(directory + "/actordata") &&
        FileExists(directory + "/moviedata")) {
      return directory;
    }
  }
  return "";
}

void CheckPath(Test* test, const std::string& source,
               const std::string& target, const Path& expected) {
  const std::string dataDirectory = FindDataDirectory();
  ASSERT_FALSE(dataDirectory.empty());
  IMDB database(dataDirectory);
  ASSERT_TRUE(database.good());
  std::vector<Film> credits;
  ASSERT_TRUE(database.getCredits(source, credits));
  ASSERT_TRUE(database.getCredits(target, credits));
  const Path actual = FindShortestPath(database, source, target);
  ASSERT_PATH_EQ(actual, expected);
}

}  // namespace

TEST(JackNicholson_To_MerylStreep) {
  CheckPath(test, "Jack Nicholson", "Meryl Streep",
            ExpectedPath("Jack Nicholson",
                         {{"Heartburn", 1986, "Meryl Streep"}}));
}

TEST(MaryTylerMoore_To_RedButtons) {
  CheckPath(test, "Mary Tyler Moore", "Red Buttons",
            ExpectedPath("Mary Tyler Moore",
                         {{"Change of Habit", 1969, "Regis Toomey"},
                          {"C.H.O.M.P.S.", 1979, "Red Buttons"}}));
}

TEST(KevinBacon_To_MerylStreep) {
  CheckPath(test, "Kevin Bacon", "Meryl Streep",
            ExpectedPath(
                "Kevin Bacon",
                {{"Boffo! Tinseltown's Bombs and Blockbusters", 2006,
                  "Meryl Streep"}}));
}

TEST(JerryCain_To_KevinBacon) {
  CheckPath(
      test, "Jerry Cain", "Kevin Bacon",
      ExpectedPath("Jerry Cain",
                   {{"No Rules", 2005, "Dian Bachar"},
                    {"Adventures of Rocky & Bullwinkle, The", 2000,
                     "Robert De Niro"},
                    {"Sleepers", 1996, "Kevin Bacon"}}));
}

TEST(CarolEby_To_DebraMuubu) {
  CheckPath(test, "Carol Eby", "Debra Muubu",
            ExpectedPath("Carol Eby",
                         {{"Bottega dell'orefice, La", 1988,
                           "Burt Lancaster"},
                          {"Scalphunters, The", 1968, "Tony Epper (I)"},
                          {"Alien from L.A.", 1988, "Debra Muubu"}}));
}

TEST(MerylStreep_To_KevinBacon) {
  CheckPath(
      test, "Meryl Streep", "Kevin Bacon",
      ExpectedPath("Meryl Streep",
                   {{"Boffo! Tinseltown's Bombs and Blockbusters", 2006,
                     "Kevin Bacon"}}));
}

TEST(BradPitt_To_DutaSkhirtladze) {
  CheckPath(test, "Brad Pitt", "Duta Skhirtladze",
            ExpectedPath("Brad Pitt",
                         {{"Abby Singer", 2003, "Jeff Goldblum"},
                          {"Fay Grim", 2006, "Mark Zak"},
                          {"Wolfsschlucht", 2003, "Duta Skhirtladze"}}));
}

TEST(MerylStreep_To_AngelinaJolie) {
  CheckPath(test, "Meryl Streep", "Angelina Jolie",
            ExpectedPath("Meryl Streep",
                         {{"Adaptation.", 2002, "Nicolas Cage"},
                          {"Gone in Sixty Seconds", 2000,
                           "Angelina Jolie"}}));
}

TEST(NoPath_FromDKusov_To_YamingGe) {
  CheckPath(test, "D. Kusov", "Yaming Ge", ExpectedPath("D. Kusov", {}));
}

int main(int argc, char* argv[]) {
  LUnitOpts options;
  LUnitOptsInit(&options, argc, argv);

  TEST_SUITE_WITH_WEIGHT(checker_cases, 1.0);
  ADD_TEST(checker_cases, JackNicholson_To_MerylStreep);
  ADD_TEST(checker_cases, MaryTylerMoore_To_RedButtons);
  ADD_TEST(checker_cases, KevinBacon_To_MerylStreep);
  ADD_TEST(checker_cases, JerryCain_To_KevinBacon);
  ADD_TEST(checker_cases, CarolEby_To_DebraMuubu);
  ADD_TEST(checker_cases, MerylStreep_To_KevinBacon);
  ADD_TEST(checker_cases, BradPitt_To_DutaSkhirtladze);
  ADD_TEST(checker_cases, MerylStreep_To_AngelinaJolie);
  ADD_TEST(checker_cases, NoPath_FromDKusov_To_YamingGe);

  TestSuite* suites[] = {&checker_cases};
  ProcessTestSuites(1, suites, &options);
  TestSuiteDispose(&checker_cases);
  return 0;
}
