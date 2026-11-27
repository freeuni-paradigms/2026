
# Robots - ნაკადები (50 ქულა)
თქვენი ამოცანაა იმპლემენტაცია გაუკეთოთ Competition სტრუქტურას რომელიც გამოიყენება რობოტებს შორის შეჯიბრის მოსაწყობად:

* void CompetitionInit(Competition* comp) - ინიციალიზაცია უნდა გაუკეთოს გადმოცემულ შეჯიბრის სტრუქტურას.
* void CompetitionStart(Competition* comp) - უნდა გაუშვას შეჯიბრის მენეჯმენტისთვის საჭირო მთავარი ნაკადი. ამ ნაკადმა მონიტორინგი უნდა გაუკეთოს შეჯიბრებაში ჩართულ რობოტებს და როგორც კი (ნებისმიერი) ორი რობოტი იქნება თავისუფალი ისინი უნდა შეაჯიბროს ერთმანეთს RobotFightWith ფუნქციის გამოყენებით. გამარჯვებული რობოტი უკან უნდა დაბრუნდეს რიგში რათა სხვა რობოტს შეეჯიბროს. გარანტირებული ხართ რომ თუ RobotFightWith(x, y) == !RobotFightWith(y, x)
* const Robot* CompetitionStop(Competition* comp) - კლიენტის ამ ფუნქციის გამოძახების შემდეგ შეჯიბრებამ უნდა შეწყვიტოს ახალი რობოტების მიღება, მაგრამ უნდა დაელოდოს მანამ სანამ უკვე მიღებული რობოტებისგან მხოლოდ ერთი გამარჯვებული არ დარჩება, და უკან უნდა დააბრუნოს ამ გამარჯვებული რობოტზე მიმთითებელი.
* bool CompetitionAddParticipant(Competition* comp, const Robot* robot) - შეჯიბრებაში უნდა დაამატოს გადმოცემული რობოტი თუ ახალი წევრების მიღება არ არის შეწყვეტილი (იხილეთ CompetitionStop ზემოთ). უნდა დააბრუნოს true თუ რობოტი დაემატა შეჯიბრში და false წინააღმდეგ შემთხვევაში. **1000-ზე მეტი რობოტი არასდროს დაემატება შეჯიბრში.**

**უფლება გაქვთ შეცვალოთ მხოლოდ competition.h და competition.c ფაილები.**

**ნაშრომი შეფასდება ხელით კომპილაციის გარეშე.**

## შეიძლება დაგჭირდეთ შემდეგი POSIX სტრუქტურები და ფუნქციები:
### semaphore.h
* sem_t: სემაფორის ტიპის სტრუქტურა
* int sem_init(sem_t *sem, int pshared, unsigned int value); -- initializes the unnamed semaphore at the address pointed to by sem. The value argument specifies the initial value for the semaphore.
* int sem_destroy(sem_t *sem); -- destroys the unnamed semaphore at the address pointed to by sem.
* int sem_post(sem_t *sem); -- increments (unlocks) the semaphore pointed to by sem.  If the semaphore's value consequently becomes greater than zero, then another process or thread blocked in a sem_wait(3) call will be woken up and proceed to lock the semaphore.
* int sem_wait(sem_t *sem); -- decrements (locks) the semaphore pointed to by sem.  If the semaphore's value is greater than zero, then the decrement proceeds, and the function returns, immediately.  If the semaphore currently has the value zero, then the call blocks until either it becomes possible to perform the decrement (i.e., the semaphore value rises above zero), or a signal handler interrupts the call.

### pthread.h
* pthread_t: ნაკატის ტიპის სტრუქტურა
* int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg); -- The pthread_create() function starts a new thread in the calling process.  The new thread starts execution by invoking start_routine(); arg is passed as the sole argument of start_routine().
* int pthread_join(pthread_t thread, void **retval); -- The pthread_join() function waits for the thread specified by thread to terminate.  If that thread has already terminated, then pthread_join() returns immediately.  The thread specified by thread must be joinable.
* void pthread_exit(void *retval); -- The pthread_exit() function terminates the calling thread and returns a value via retval that (if the thread is joinable) is available to another thread in the same process that calls pthread_join(3).
* pthread_mutex_t: მუტექსის ტიპის სტრუქტურა
* int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr); -- The pthread_mutex_init() function initialises the mutex referenced by mutex with attributes specified by attr. If attr is NULL, the default mutex attributes are used; the effect is the same as passing the address of a default mutex attributes object. Upon successful initialisation, the state of the mutex becomes initialised and unlocked.
* int pthread_mutex_destroy(pthread_mutex_t *mutex); -- The pthread_mutex_destroy() function destroys the mutex object referenced by mutex; the mutex object becomes, in effect, uninitialised. An implementation may cause pthread_mutex_destroy() to set the object referenced by mutex to an invalid value. A destroyed mutex object can be re-initialised using pthread_mutex_init(); the results of otherwise referencing the object after it has been destroyed are undefined.
* int pthread_mutex_lock(pthread_mutex_t *mutex); -- The mutex object referenced by mutex shall be locked by calling pthread_mutex_lock(). If the mutex is already locked, the calling thread shall block until the mutex becomes available. This operation shall return with the mutex object referenced by mutex in the locked state with the calling thread as its owner.
* int pthread_mutex_unlock(pthread_mutex_t *mutex); -- he pthread_mutex_unlock() function shall release the mutex object referenced by mutex. The manner in which a mutex is released is dependent upon the mutex's type attribute. If there are threads blocked on the mutex object referenced by mutex when pthread_mutex_unlock() is called, resulting in the mutex becoming available, the scheduling policy shall determine which thread shall acquire the mutex.
