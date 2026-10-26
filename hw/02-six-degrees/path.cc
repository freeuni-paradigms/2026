#include "path.h"

#include <iostream>

Path::Path(const std::string& player) : startPlayer(player) {}

int Path::getLength() const {
  return static_cast<int>(links.size());
}

const std::string& Path::getLastPlayer() const {
  return links.empty() ? startPlayer : links.back().player;
}

void Path::addConnection(const Film& movie, const std::string& player) {
  links.emplace_back(movie, player);
}

void Path::undoConnection() {
  if (!links.empty()) {
    links.pop_back();
  }
}

void Path::reverse() {
  Path reversed(getLastPlayer());
  for (int i = static_cast<int>(links.size()) - 1; i > 0; --i) {
    reversed.addConnection(links[static_cast<std::size_t>(i)].movie,
                           links[static_cast<std::size_t>(i - 1)].player);
  }
  if (!links.empty()) {
    reversed.addConnection(links.front().movie, startPlayer);
  }
  *this = reversed;
}

bool Path::operator==(const Path& other) const {
  if (startPlayer != other.startPlayer || links.size() != other.links.size()) {
    return false;
  }
  for (std::size_t i = 0; i < links.size(); ++i) {
    if (!(links[i].movie == other.links[i].movie) ||
        links[i].player != other.links[i].player) {
      return false;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& output, const Path& value) {
  if (value.links.empty()) {
    return output << "[Empty path]" << std::endl;
  }

  output << '\t' << value.startPlayer << " was in ";
  for (std::size_t i = 0; i < value.links.size(); ++i) {
    output << '\"' << value.links[i].movie.title << "\" ("
           << value.links[i].movie.year << ") with "
           << value.links[i].player << '.' << std::endl;
    if (i + 1 < value.links.size()) {
      output << '\t' << value.links[i].player << " was in ";
    }
  }
  return output;
}
