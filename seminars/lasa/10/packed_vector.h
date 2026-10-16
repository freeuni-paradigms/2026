#ifndef PARADIGMS_PACKED_VECTOR_H_
#define PARADIGMS_PACKED_VECTOR_H_

typedef void(*MapFn)(int index, int size, void* elem, void* aux);
typedef void(*FreeFn)(void* addr);

typedef struct {
    void * base; // int elem_size | Freefn frefn | elem
    int logLen;
    int size;
} Vector;

// ინიციალიზაცია უნდა გაუეკთოს მოცემულ ვექტორს.
void VectorInit(Vector* v);
// უნდა გაათავისუფლოს ვექტორის მიერ გამოყენებული დინამიური მეხსიერება.
void VectorDestroy(Vector* v);
// მოცემულ ინდექსზე უნდა ჩაამატოს elem_size ზომის (ბაიტებში) ელემენტი
// რომელიც ინახება elem მისამართზე, ხოლო free_fn ფუნქცია უნდა შეინახოთ და მომავალში
// გამოიყენოთ ამ ელემენტის მეხსიერების გასათავისუფლებლად (როცა ამის საჭიროება დადგება).
// გარანტირებული ხართ რომ elem_size > 0.
// ახალი ელემენტის ჩამატებამდე მოცემულ ინდექსზე და მის მარჯვნივ მყოფი ყველა ელემენტი
// უნდა ჩაჩოჩდეს მარჯვნივ.
void VectorInsert(Vector* v, int index, int elem_size, void* elem, FreeFn free_fn);
// მოცემული ინდექსის ელემენტი უნდა ამოაგდოს სიიდან. მის მარჯვნივ მყოფი ყველა
// ელემენტი უნდა ჩამოცოცდეს მარცხნივ.
void VectorRemove(Vector* v, int index);
// უკან უნდა დააბრუნოს მოცემულ ინდექსზე შენახული ელემენტის მისამართი, ხოლო elem_size
// ცვლადში უნდა ჩაწეროს ამ ელემენტის ზომა ბაიტებში.
void* VectorGet(Vector* v, int index, int* elem_size);
// უნდა მოაბრუნოს ვექტორი მოცემულ ინდექსზე.
// მობრუნებამდე index პოზიციაზე მყოფი უნდა აღმოჩნდეს 0 პოზიციაზე მობრუნების შემდეგ
// index+1 უნდა აღმოჩნდეს 1 პოზიციაზე
// ...
// 0-ოვანი უნდა აღმოჩნდეს n-index პოზიციაზე
// ...
// სადაც n განსაზღვრავს ვექტორში შენახული ელემენტების რაოდენობას.
void VectorRotate(Vector* v, int index);
// ვექტორში შენახული ყველა ელემენტისთვის უნდა გამოიძახოს
// გადმოცემული map_fn ფუნქცია, რომელსაც არგუმენტებად უნდა გადასცეს:
// * მიმდინარე ელემენტის ინდექსი
// * ამ ელემენტის ზომა ბაიტებში
// * ამ ელემენტის მნიშვნელობის მისამართი
// * და VectorMap-ისთვის კლიენტის მიერ გადმოცემული aux მნიშვნელობა.
void VectorMap(Vector* v, MapFn map_fn, void* aux);

#endif // PARADIGMS_PACKED_VECTOR_H_
