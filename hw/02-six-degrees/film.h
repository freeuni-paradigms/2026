#ifndef _PARADIGMS_HW_SIX_DEGREES_FILM_H_
#define _PARADIGMS_HW_SIX_DEGREES_FILM_H_

#include <string>

typedef struct Film {
  std::string title;
  int year;

  bool operator==(const Film& other) const {
    return title == other.title && year == other.year;
  }

  bool operator<(const Film& other) const {
    return title < other.title ||
           (title == other.title && year < other.year);
  }
} Film;

#endif  // _PARADIGMS_HW_SIX_DEGREES_FILM_H_
