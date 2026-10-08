#ifndef _PARADIGMS_HW_SIX_DEGREES_PATH_H_
#define _PARADIGMS_HW_SIX_DEGREES_PATH_H_

#include <iosfwd>
#include <string>
#include <vector>

#include "film.h"

class Path {
 public:
  explicit Path(const std::string& startPlayer);

  int getLength() const;
  const std::string& getLastPlayer() const;

  void addConnection(const Film& movie, const std::string& player);
  void undoConnection();
  void reverse();

  bool operator==(const Path& other) const;

  friend std::ostream& operator<<(std::ostream& output, const Path& value);

 private:
  struct connection {
    Film movie;
    std::string player;

    connection(const Film& movie, const std::string& player)
        : movie(movie), player(player) {}
  };

  std::string startPlayer;
  std::vector<connection> links;
};

#endif  // _PARADIGMS_HW_SIX_DEGREES_PATH_H_
