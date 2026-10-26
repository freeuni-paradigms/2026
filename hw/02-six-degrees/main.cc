#include <iostream>
#include <string>
#include <vector>

#include "six-degrees.h"

namespace {

std::string PromptForActor(const std::string& prompt, const IMDB& database) {
  std::string response;
  while (true) {
    std::cout << prompt << " [or <enter> to quit]: ";
    if (!std::getline(std::cin, response) || response.empty()) {
      return "";
    }

    std::vector<Film> credits;
    if (database.getCredits(response, credits)) {
      return response;
    }
    std::cout << "We couldn't find \"" << response
              << "\" in the movie database. Please try again." << std::endl;
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [data-directory]\n";
    return 1;
  }

  const std::string dataDirectory =
      argc == 2 ? argv[1] : "data/little-endian";
  IMDB database(dataDirectory);
  if (!database.good()) {
    std::cout << "Failed to properly initialize the imdb database."
              << std::endl;
    std::cout << "Please check to make sure the source files exist and that "
                 "you have permission to read them."
              << std::endl;
    return 1;
  }

  while (true) {
    const std::string source = PromptForActor("Actor or actress", database);
    if (source.empty()) {
      break;
    }
    const std::string target =
        PromptForActor("Another actor or actress", database);
    if (target.empty()) {
      break;
    }

    if (source == target) {
      std::cout << "Good one.  This is only interesting if you specify two "
                   "different people."
                << std::endl;
      continue;
    }

    const Path result = FindShortestPath(database, source, target);
    if (result.getLength() == 0) {
      std::cout << std::endl
                << "No path between those two people could be found."
                << std::endl << std::endl;
    } else {
      std::cout << std::endl << result << std::endl;
    }
  }

  std::cout << "Thanks for playing!" << std::endl;
  return 0;
}
