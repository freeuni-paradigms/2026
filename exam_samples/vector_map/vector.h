#ifndef PARADIGMS_VECTOR_MAP_VECTOR_H_
#define PARADIGMS_VECTOR_MAP_VECTOR_H_

typedef void(*FreeFn)(void* elem);
typedef void(*MapFn)(void* elem, void* aux);

typedef struct {
  char __PADDING[100];
  void* base_;
  int elem_size_;
  int log_len_;
  int alloc_len_;
} Vector;

// ინიციალიზაციას უკეთებს გადმოცემულ ვექტორს, elem_size აღნიშნავს შესანახი ელემენტების ზომას ბაიტებში.
void VectorInit(Vector* v, int elem_size);
// ათავისუფლებს ვექტორის მიერ გამოყენებულ მეხსიერებას.
void VectorDestroy(Vector* v);
// ვექტორის ბოლოს ამატებს გადმოცემულ ახალ ელემენტს. არ იღებს გადმოცემულ მეხსიერებაზე მფლობელობას.
void VectorAppend(Vector* v, void* elem);
// ვექტორიდან აგდებს მოცემული ინდექსის მქონდე ელემენტს.
void VectorRemove(Vector* v, int index);
// ვექტორის ყოველი ელემენტისთვის, დაწყებული 0 ინდექსიდან, იძახებს გადმოცემულ map_fn ფუნქციას
// და არგუმენტებად გადასცემს მიმდინარე ელემენტის მისამართს და aux ცვლადს.
void VectorMap(Vector* v, MapFn map_fn, void* aux);

#endif // PARADIGMS_VECTOR_MAP_VECTOR_H_
