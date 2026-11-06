# SortedList

თქვენი ამოცანაა იმპლემენტაცია გაუკეთოთ SortedList მოცანემთა სტრუქტურას რომელსაც უნდა შეეძლოს (key, value) წყვილების შენახვა და უნდა ქონდეს შემდეგი ინტერფეისი:


* void SortedListInit(SortedList *l, int key_size, int value_size, CompareFn key_cmp_fn, FreeFn key_free_fn, FreeFn value_free_fn); -- ინიციალიზაციას უნდა უკეთებდეს გადმოცემულ სიას.
* void SortedListDestroy(SortedList *l); -- უნდა ანთავისიფულებდეს სიის მიერ გამოყენებულ დინამიურ მეხსიერებას.
* void SortedListAdd(SortedList *l, void *key, void *value); -- უნდა ამატებდეს მოცემულ (key, value) წყვილს სიაში. თუ სიაში მოცემული გასაღები უკვე გვხვდება მაშინ მასთან ასოცირებული მიმდინარე მნიშვნელობა უნდა იშლებოდეს და მის ადგილას ახალი მნიშვნელობა უნდა ჩაიწეროს. **იღებს მფლობელობას ორივე key და value მისამართებზე.**
* bool SortedListRemove(SortedList *l, void *key); -- სიიდან უნდა აგდებდეს მოცემულ გასაღებს და მასთან ასოცირებულ მნიშვნელობას. თუ გასაღები მოიძებნა უნდა აბრუნებდეს true-ს, წინააღმდეგ შემთხვევაში კი false-ს. **არ იღებს მფლობელობას key მისამართებზე.**
* const void *SortedListGet(SortedList *l, void *key); -- უნდა აბრუნებდეს მოცემულ გასაღებთან ასოცირებული მნიშვნელობის მიმთითებელს. NULL-ს თუ ასეთი გასაღები ვერ მოიძებნა. **არ იღებს მფლობელობას key მისამართებზე. დაბრუნებული მისამართის მფლობელი რჩება გამოყენებულ SortedList ობიექტს.**

SortedList სტრუქტურის შიგთავისი უკვე აღწერილია sorted_list.h ფაილში და მისი შეცვლის უფლება არ გაქვთ (ზოგადად sorted_list.h ფაილის ცვლილების უფლება არ გაქვთ). მისი შიგთავსი შემდეგნაირია:
* void *base; -- აქ უნდა შეინახოთ გადმოცემული (key, value) წყვილები.
* int log_len; -- შეგიძლიათ გამოიყენოთ სიაში შენახული ელემენტების რაოდენობის შესანახად.
* int alloc_len; -- შეგიძლიათ შეინახოთ თუ რამდენი ელემენტის შენახვა შეუძლია სიას.
* int key_size; -- გასაღების ზომა ბაიტებში.
* int value_size; -- მნიშვნელობის ზომა ბაიტებში.
* CompareFn key_cmp_fn; -- გასაღებების შემდარებელი ფუნქცია.
* FreeFn key_free_fn; -- გასაღებების მეხსიერების გამათავისუფლებელი ფუნქცია.
* FreeFn value_free_fn; მნიშვნელობების მეხსიერების გამათავისუფლებელი ფუნქცია.

### ტესტირება
ფაილების კომპილაციისთვის გაუშვით **gcc tests.c sorted_list.c**<br/>
ხოლო დაკომპილირებული ფაილის გასაშვებად: **a.exe**

### შეფასება
ტესტები შემდეგ კატეგორიებად იქნება დაყოფილი:
* 35% - მხოლოდ შეამოწმებს SortedListInit, SortedListDestroy, SortedListAdd და SortedListGet ფუნქციებს სადაც SortedListAdd მეთოდისთვის გადაცემული გასაღებები უნიკალურები იქნებიან.
* 35% - ზემოთ ხსენებულ მეთოდებთან ერთად შემოწმდება SortedListRemove, ანუ ყველა მეთოდი. ამ შემთხვევაშიც SortedListAdd მეთოდისთვის გადაცემული გასაღებები უნიკალურები იქნებიან.
* 30% - შემოწმდება ყველა მეთოდი. ამ შემთხვევაში SortedListAdd მეთოდისთვის გადაცემული გასაღებები უნიკალურები არ იქნებიან.

**სიის ზომა არასდროს გახდება 10^4-ზე მეტი.**

**თითოეულ ტესტზე ინდივიდუალურად შემოწმდება თქვენი ამოხსნა სწორად იყენებს თუ არა მეხსიერებას. ყოველ წარმატებულად გავლილ ტესტზე რომლის შესრულების დროსაც თქვენი ნამუშევარი მეხსიერებას არასწორად გამოიყენებს დაგაკლდებათ ტესტისთვის განკუთვნილი ქულის 15%. სწორად გამოყენებაში იგულისხმება რომე აპლიკაცია უნდა იყენებდეს მხოლოდ მისთვის გამოყოფილ მეხსიერებას და აპლიკაციის დასრულებამდე ყველა დინამიურად გამოყოფილი მეხსერება უნდა გაათავისუფლოთ.**

### შეიძლება დაგჭირდეთ შემდეგი ფუნქციები სტანდარტული ბიბლიოთეკიდან:

#### <stdlib.h>
* void* malloc( size_t size ); -- Allocates size bytes of uninitialized storage.
* void *realloc( void *ptr, size_t new_size ); -- Reallocates the given area of memory. It must be previously allocated by malloc(), calloc() or realloc() and not yet freed with a call to free or realloc. Otherwise, the results are undefined.
* void free( void* ptr ); -- Deallocates the space previously allocated by malloc(), calloc(), aligned_alloc, (since C11) or realloc().

#### <string.h>
* void* memcpy( void *dest, const void *src, size_t count ); -- Copies count characters from the object pointed to by src to the object pointed to by dest. Both objects are interpreted as arrays of unsigned char.
* void* memmove( void* dest, const void* src, size_t count ); -- Copies count characters from the object pointed to by src to the object pointed to by dest. Both objects are interpreted as arrays of unsigned char. The objects may overlap: copying takes place as if the characters were copied to a temporary character array and then the characters were copied from the array to dest.
 The behavior is undefined if access occurs beyond the end of the dest array. The behavior is undefined if either dest or src is a null pointer.
* char * strdup(const char *str1); -- Returns a pointer to a null-terminated byte string, which is a duplicate of the string pointed to by str1. The returned pointer must be passed to free to avoid a memory leak.
* char *strndup(const char *str, size_t size); -- Returns a pointer to a null-terminated byte string, which contains copies of at most size bytes from the string pointed to by str. If the null terminator is not encountered in the first size bytes, it is added to the duplicated string.