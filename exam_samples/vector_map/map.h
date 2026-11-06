// !!! არ გაქვთ ამ ფაილის ცვლილების უფლება !!!

#ifndef PARADIGMS_VECTOR_MAP_MAP_H_
#define PARADIGMS_VECTOR_MAP_MAP_H_

#include "vector.h"

typedef int(*CmpFn)(void* a, void* b);
typedef void(*MapMapFn)(void* key, void* value, void* aux);

typedef struct {
  // !!! არ გაქვთ უფლება შეცვალოთ სტრაქტის შიგთავსი !!!
  Vector v;
  int key_size;
  int value_size;
  CmpFn cmp_fn;
  FreeFn key_free_fn;
  FreeFn value_free_fn;
} Map;

// ინიციალიზაცია უნდა გაუკეთოს გადმოცემულ მეპს, სადაც key_size არის გასაღებების ზომა ბაიტებში,
// value_size არის მნიშნველობების ზომა ბაიტებში, key_cmp_fn არის გასაღებების შემადარებელი ფუნქცია,
// key_free_fn და value_free_fn უნდა გამოიყენოთ გასაღებების და მნიშნველობების მეხსიერებების გასათავისუფლებლად.
void MapInit(Map* m, int key_size, int value_size, CmpFn key_cmp_fn, FreeFn key_free_fn, FreeFn value_free_fn);
// უნდა გაათავისუფლოს მეპის მიერ გამოყენებული მეხსიერება.
void MapDestroy(Map* m);
// მოცემულ გასაღებზე უნდა დაიმახსოვროს მოცემული მნიშვნელობა.
// მფლობელობას იღებს ორივე მისამართზე.
void MapSet(Map* m, void* key, void* value);
// უნდა დააბრუნოს მოცემულ გასაღებზე შენახული მნიშვნელობის მისამართი.
void* MapGet(Map* m, void* key);
// მეპიდან უნდა ამოაგდოს მოცემული გასაღები და მასთან ასოცირებული მნიშვნელობა.
// თუ გასაღები მეპში არ გვხვდება მაშინ არაფერი არ უნდა შეიცვალოს მეპში.
void MapRemove(Map* m, void* key);
// მეპში შენახული ყოველი (key, value) წყვილებისთვის უნდა გამოიძახოს გადმოცემული map_fn ფუნქცია
// და არგუმენტებად გადასცვეს მიმდინარე გასაღების მისამართი, მასთან ასოცირებული მნიშვნელობის მისამართი და aux.
void MapMap(Map* m, MapMapFn map_fn, void* aux);

#endif // PARADIGMS_VECTOR_MAP_MAP_H_
