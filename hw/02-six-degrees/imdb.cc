#include "imdb.h"

#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const char* const IMDB::kActorFileName = "actordata";
const char* const IMDB::kMovieFileName = "moviedata";

IMDB::IMDB(const std::string& directory)
    : actorFile(nullptr),
      movieFile(nullptr),
      actorInfo{-1, 0, nullptr},
      movieInfo{-1, 0, nullptr} {
  actorFile = acquireFileMap(directory + "/" + kActorFileName, actorInfo);
  movieFile = acquireFileMap(directory + "/" + kMovieFileName, movieInfo);
}

IMDB::~IMDB() {
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

bool IMDB::good() const {
  return actorFile != nullptr && movieFile != nullptr;
}

const void* IMDB::acquireFileMap(const std::string& fileName, fileInfo& info) {
  info = {-1, 0, nullptr};
  info.fd = open(fileName.c_str(), O_RDONLY);
  if (info.fd == -1) {
    return nullptr;
  }

  struct stat stats {};
  if (fstat(info.fd, &stats) == -1 || stats.st_size <= 0) {
    close(info.fd);
    info.fd = -1;
    return nullptr;
  }

  info.fileSize = static_cast<std::size_t>(stats.st_size);
  void* mapping =
      mmap(nullptr, info.fileSize, PROT_READ, MAP_PRIVATE, info.fd, 0);
  if (mapping == MAP_FAILED) {
    close(info.fd);
    info = {-1, 0, nullptr};
    return nullptr;
  }

  info.fileMap = mapping;
  return info.fileMap;
}

void IMDB::releaseFileMap(fileInfo& info) {
  if (info.fileMap != nullptr) {
    munmap(const_cast<void*>(info.fileMap), info.fileSize);
  }
  if (info.fd != -1) {
    close(info.fd);
  }
  info = {-1, 0, nullptr};
}

bool IMDB::getCredits(const std::string& player,
                      std::vector<Film>& films) const {
  // IMPLEMENT
  return false;
}

bool IMDB::getCast(const Film& movie,
                   std::vector<std::string>& players) const {
  // IMPLEMENT
  return false;
}
