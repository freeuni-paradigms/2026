#ifndef _PARADIGMS_HW_SIX_DEGREES_IMDB_H_
#define _PARADIGMS_HW_SIX_DEGREES_IMDB_H_

#include <cstddef>
#include <string>
#include <vector>

#include "film.h"

class IMDB {
 public:
  // Opens the compact actor and movie files in directory.
  explicit IMDB(const std::string& directory);

  // Returns whether both database files were opened and mapped successfully.
  bool good() const;

  // Replaces films with player's credits. Returns false and leaves films empty
  // when player is not present.
  bool getCredits(const std::string& player, std::vector<Film>& films) const;

  // Replaces players with movie's cast. Returns false and leaves players empty
  // when movie is not present.
  bool getCast(const Film& movie, std::vector<std::string>& players) const;

  ~IMDB();

 private:
  static const char* const kActorFileName;
  static const char* const kMovieFileName;

  const void* actorFile;
  const void* movieFile;

  struct fileInfo {
    int fd;
    std::size_t fileSize;
    const void* fileMap;
  } actorInfo, movieInfo;

  static const void* acquireFileMap(const std::string& fileName,
                                    fileInfo& info);
  static void releaseFileMap(fileInfo& info);

  IMDB(const IMDB&) = delete;
  IMDB& operator=(const IMDB&) = delete;
};

#endif  // _PARADIGMS_HW_SIX_DEGREES_IMDB_H_
