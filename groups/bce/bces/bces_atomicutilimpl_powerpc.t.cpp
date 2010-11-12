// bces_atomicutilimpl_powerpc.t.cpp  -*-C++-*-

#include <bces_atomicutilimpl_powerpc.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bslma_default.h> // for usage examples

#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_algorithm.h>

#include <bsl_c_stdlib.h> // atoi
using bsl::cout;
using bsl::endl;
using bsl::flush;

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
//
// [2 ] initInt(bces_AtomicUtilImpl...::Int *aInt);
// [2 ] initInt64(bces_AtomicUtilImpl...::Int64 *aInt);
// [2 ] initPointer(bces_AtomicUtilImpl...::Pointer *aPointer);
// [6 ] initSpinLock(bces_AtomicUtilImpl...::SpinLock *aSpin);
// [3 ] addInt(bces_AtomicUtilImpl...::Int *aInt, int value);
// [5 ] incrementInt(bces_AtomicUtilImpl...::Int *aInt);
// [5 ] decrementInt(bces_AtomicUtilImpl...::Int *aInt);
// [4 ] swapInt64(bces_AtomicUtilImpl...::Int *aInt, int value);
// [4 ] testAndSwapInt(bces_AtomicUtilImpl...::Int *, int, int);
// [2 ] setInt(bces_AtomicUtilImpl...::Int *aInt, int value);
// [2 ] getInt(const bces_AtomicUtilImpl...::Int &aInt);
// [3 ] addIntNv(bces_AtomicUtilImpl...::Int *aInt, int val);
// [5 ] incrementIntNv(bces_AtomicUtilImpl...::Int *aInt);
// [5 ] decrementIntNv(bces_AtomicUtilImpl...::Int *aInt);
// [3 ] addInt64(bces_AtomicUtilImpl...::Int64 *, bsls_PlatformUtil::Int64);
// [5 ] incrementInt64(bces_AtomicUtilImpl...::Int64 *aInt);
// [5 ] decrementInt64(bces_AtomicUtilImpl...::Int64 *aInt);
// [4 ] swapInt64(bces_AtomicUtilImpl...::Int64 *, bsls_PlatformUtil::Int64);
// [4 ] testAndSwapInt64(bces_AtomicUtilImpl...::Int64 *,
//                       bsls_PlatformUtil::Int64,
//                       bsls_PlatformUtil::Int64);
// [3 ] addInt64Nv(bces_AtomicUtilImpl...::Int64 *, bsls_PlatformUtil::Int64);
// [5 ] incrementInt64Nv(bces_AtomicUtilImpl...::Int64 *);
// [5 ] decrementInt64Nv(bces_AtomicUtilImpl...::Int64 *);
// [2 ] setInt64(bces_AtomicUtilImpl...::Int64 *, bsls_PlatformUtil::Int64);
// [2 ] getInt64(const bces_AtomicUtilImpl...::Int64 &aInt);
// [2 ] getPtr(const bces_AtomicUtilImpl...::Pointer &aPointer);
// [2 ] setPtr(bces_AtomicUtilImpl...::Pointer *aPointer, void *value);
// [4 ] swapPtr(bces_AtomicUtilImpl...::Pointer *aPointer, void *value);
// [4 ] testAndSwapPtr(bces_AtomicUtilImpl...::Pointer *, void *, void *);
// [6 ] spinLock(bces_AtomicUtilImpl...::SpinLock *aSpin);
// [6 ] spinTryLock(bces_AtomicUtilImpl...::SpinLock *aSpin, int retries);
// [6 ] spinUnlock(bces_AtomicUtilImpl...::SpinLock *aSpin);
//-----------------------------------------------------------------------------
// [1 ] Breathing test
//-----------------------------------------------------------------------------
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

#ifdef BSLS_PLATFORM__CPU_POWERPC

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc> Obj;

const int INT_SWAPTEST_VALUE1 = 0x33ff33ff;
const int INT_SWAPTEST_VALUE2 = 0xff33ff33;

const bsls_PlatformUtil::Int64 INT64_SWAPTEST_VALUE1 = 0x33ff33ff33ff33ffLL;
const bsls_PlatformUtil::Int64 INT64_SWAPTEST_VALUE2 = 0xff33ff33ff33ff33LL;

const void *POINTER_SWAPTEST_VALUE1 = (void*)0x33ff33ff;
const void *POINTER_SWAPTEST_VALUE2 = (void*)0xff33ff33;

extern "C" {
typedef void *(*THREAD_ENTRY)(void *arg);
}

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
typedef HANDLE my_thread_t;
#else
#include <pthread.h>
#include <bsl_c_sys_time.h>
typedef pthread_t my_thread_t;
#endif

struct Case8
{
    Obj::Int  *d_value_p;
    int        d_n;
    int        d_m;
};

class my_Mutex {
    // This class implements a cross-platform mutual exclusion primitive
    // similar to posix mutexes.
#ifdef BSLS_PLATFORM__OS_WINDOWS
    HANDLE d_mutex;
#else
    pthread_mutex_t d_mutex;
#endif

  public:
    my_Mutex();
        // Construct an 'my_Mutex' object.
    ~my_Mutex();
        // Destroy an 'my_Mutex' object.

    void lock();
        // Lock this mutex.

    void unlock();
        // Unlock this mutex;
};

class my_Conditional {
    // This class implements a cross-platform waitable state indicator used for
    // testing.  It has two states, signaled and non-signaled.  Once
    // signaled('signal'), the state will persist until explicitly 'reset'.
    // Calls to wait when the state is signaled, will succeed immediately.
#ifdef BSLS_PLATFORM__OS_WINDOWS
    HANDLE d_cond;
#else
    pthread_mutex_t d_mutex;
    pthread_cond_t  d_cond;
    volatile int   d_signaled;
#endif

  public:
    my_Conditional();
    ~my_Conditional();

    void reset();
        // Reset the state of this indicator to non-signaled.

    void signal();
        // Signal the state of the indicator and unblock any thread waiting
        // for the state to be signaled.

    void wait();
        // Wait until the state of this indicator becomes signaled.  If the
        // state is already signaled then return immediately.

    int  timedWait(int timeout);
        // Wait until the state of this indicator becomes signaled or until or
        // for the specified 'timeout'(in milliseconds).  Return 0 if the state
        // is signaled, non-zero if the timeout has expired.  If the state is
        // already signaled then return immediately.
};

struct IntTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    int            d_addVal;
    Obj::Int      *d_int_p;
};

struct Int64TestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    bsls_PlatformUtil::Int64 d_addVal;
    Obj::Int64    *d_int_p;
};

struct StressTestArgs {
    my_Conditional d_barrier;
    bsl::vector<bsls_PlatformUtil::Int64> d_values;
    Obj::Int      d_done;
    Obj::Int64    d_value;
    int           d_numValues;

    StressTestArgs() {
        Obj::initInt(&d_done, 0);
        Obj::initInt64(&d_value, 0);
    }
};

struct IntSwapTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Obj::Int      *d_int_p;
};

struct Int64SwapTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Obj::Int64     *d_int_p;
};

struct PointerTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Obj::Pointer   *d_ptr_p;
};

struct SpinLockTestThreadArgs {
    my_Conditional  d_barrier;
    my_Conditional  d_startSig;
    my_Mutex        d_mutex;
    volatile int    d_countStarted;
    int             d_iterations;
    Obj::SpinLock *d_lock_p;
    volatile int    d_count;
};

inline
my_Mutex::my_Mutex()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_mutex = CreateMutex(0,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
#endif
}

inline
my_Mutex::~my_Mutex()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    CloseHandle(d_mutex);
#else
    pthread_mutex_destroy(&d_mutex);
#endif
}

inline
void my_Mutex::lock()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(d_mutex, INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
#endif
}

inline
void my_Mutex::unlock()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    ReleaseMutex(d_mutex);
#else
    pthread_mutex_unlock(&d_mutex);
#endif
}

my_Conditional::my_Conditional()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_cond = CreateEvent(0,TRUE,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
    pthread_cond_init(&d_cond,0);
    d_signaled = 0;
#endif
}

my_Conditional::~my_Conditional()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    CloseHandle(d_cond);
#else
    pthread_cond_destroy(&d_cond);
    pthread_mutex_destroy(&d_mutex);
#endif
}

void my_Conditional::reset()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    ResetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 0;
    pthread_mutex_unlock(&d_mutex);
#endif
}

void my_Conditional::signal()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    SetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 1;
    pthread_cond_broadcast(&d_cond);
    pthread_mutex_unlock(&d_mutex);
#endif
}

void my_Conditional::wait()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(d_cond,INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
    while (!d_signaled) pthread_cond_wait(&d_cond,&d_mutex);
    pthread_mutex_unlock(&d_mutex);
#endif
}

int my_Conditional::timedWait(int timeout)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    DWORD res = WaitForSingleObject(d_cond,timeout);
    return res == WAIT_OBJECT_0 ? 0 : -1;
#else
    struct timeval now;
    struct timespec tspec;
    int res;

    gettimeofday(&now,0);
    tspec.tv_sec  = now.tv_sec + timeout/1000;
    tspec.tv_nsec = (now.tv_usec + (timeout%1000) * 1000) * 1000;
    pthread_mutex_lock(&d_mutex);
    while ((res = pthread_cond_timedwait(&d_cond,&d_mutex,&tspec)) == 0 &&
           !d_signaled) {
        ;
     }
    pthread_mutex_unlock(&d_mutex);
    return res;
#endif
}

static int myCreateThread( my_thread_t *aHandle, THREAD_ENTRY aEntry,
                           void *arg )
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    *aHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)aEntry,arg,0,0);
    return *aHandle ? 0 : -1;
#else
    return pthread_create(aHandle, 0, aEntry, arg);
#endif
}

static void  myJoinThread(my_thread_t aHandle)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(aHandle,INFINITE);
    CloseHandle(aHandle);
#else
    pthread_join(aHandle,0);
#endif
}

extern "C" {

static void *case8Thread(void *ptr)
{
    Case8 *args = (Case8*) ptr;

    for (int i = 0; i < args->d_m; ++i) {
        int value = Obj::getIntRelaxed(*args->d_value_p);
        int newValue = Obj::addIntNvRelaxed(args->d_value_p, 1);
        LOOP2_ASSERT(value, newValue,
                    newValue > value && newValue <= args->d_m * args->d_n);
    }
    return ptr;
}

static void *stressTestReaderThread(void *ptr)
{
    StressTestArgs *args = (StressTestArgs*)ptr;

    args->d_barrier.wait();

    while (0 == Obj::getInt(args->d_done)) {
        bsls_PlatformUtil::Int64 value = Obj::getInt64(args->d_value);

        bool goodValue = bsl::binary_search(args->d_values.begin(),
                                            args->d_values.end(),
                                            value);
        LOOP_ASSERT(value, goodValue);
    }

    return ptr;
}

static void *stressTestUpdateThread(void *ptr)
{
    StressTestArgs *args = (StressTestArgs*)ptr;
    unsigned int seed = 123456;

    args->d_barrier.wait();

    while (0 == Obj::getInt(args->d_done)) {
        bsls_PlatformUtil::Int64 oldvalue = Obj::getInt64(args->d_value);
        bsls_PlatformUtil::Int64 newvalue =
            args->d_values[rand_r(&seed) % args->d_numValues];
        while (oldvalue != Obj::testAndSwapInt64(&args->d_value,
                                                 oldvalue,
                                                 newvalue)) {
            oldvalue = Obj::getInt64(args->d_value);
        }
    }

    return ptr;
}

static void *incrementIntTestThread(void *ptr)
    // This function is used to test the 'incrementInt' and 'incrementIntNv'
    // functions.  It atomically increments the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementInt(args->d_int_p);
        Obj::incrementIntNv(args->d_int_p);
    }
    return ptr;
}

static void *decrementIntTestThread(void *ptr)
    // This function is used to test the 'decrementInt' and 'decrementIntNv'
    // functions.  It atomically decrements the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementInt(args->d_int_p);
        Obj::decrementIntNv(args->d_int_p);
    }
    return ptr;
}

static void *incrementInt64TestThread(void *ptr)
    // This function is used to test the 'incrementInt64' function.  It
    // atomically increments the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // incremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementInt64(args->d_int_p);
        Obj::incrementInt64Nv(args->d_int_p);
    }
    return ptr;
}

static void *decrementInt64TestThread(void *ptr)
    // This function is used to test the 'decrementInt64' function.  It
    // atomically decrements the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // decremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementInt64(args->d_int_p);
        Obj::decrementInt64Nv(args->d_int_p);
    }
    return ptr;
}

static void *addIntTestThread(void *ptr)
    // This function is used to test the 'addInt' function.  It atomically
    // adds the specified 'd_addVal' to the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addIntNv(args->d_int_p,args->d_addVal);
    }
    return ptr;
}

static void *addInt64TestThread(void *ptr)
    // This function is used to test the 'addInt64' and 'addInt64Nv' functions.
    // It atomically adds the specified 'd_addVal' to the specified 64 bit
    // atomic integer object for the specified number of iterations.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64Nv(args->d_int_p,args->d_addVal);
    }
    return ptr;
}

static void *swapIntTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::swapInt(args->d_int_p,INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *swapInt64TestThread(void *ptr)
    // This function is used to test the 'swapInt' function.
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls_PlatformUtil::Int64 oldValue=
            Obj::swapInt64(args->d_int_p,INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *testAndSwapIntTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::testAndSwapInt(args->d_int_p,INT_SWAPTEST_VALUE1,
                                INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *testAndSwapInt64TestThread(void *ptr)
    // This function is used to test the 'testAndSwapInt64' function.
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls_PlatformUtil::Int64 oldValue=
            Obj::testAndSwapInt64(args->d_int_p,INT64_SWAPTEST_VALUE1,
                                  INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *swapPtrTestThread(void *ptr)
    // This function is used to test the 'swapPtr' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::swapPtr(args->d_ptr_p,POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *testAndSwapPtrTestThread(void *ptr)
    // This function is used to test the 'testAndSwap' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::testAndSwapPtr(args->d_ptr_p,POINTER_SWAPTEST_VALUE1,
                                  POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void *spinLockTestThread(void *ptr)
    // This function is used to test the 'spinLock' and 'spinUnlock' functions.
    // It locks the spinLock, increments the specified counter, and unlocks the
    // spinlock for the specified number of iterations.  When executed by
    // multiple threads concurrently, final value of the specified int should
    // have been incremented by exactly NTHREADS * NITERATIONS.
{
    SpinLockTestThreadArgs *args =(SpinLockTestThreadArgs*)ptr;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::spinLock(args->d_lock_p);
        args->d_count++;
        Obj::spinUnlock(args->d_lock_p);
    }
    return ptr;
}

}

struct BenchmarkCaseArgs {
    Obj::Int        *d_int_p;
    int              d_numIterations;
    Obj::Int        *d_total_p;
    pthread_cond_t  *d_cond_p;
    pthread_mutex_t *d_mutex_p;
    int              d_numThreads;
    int             *d_numStarted_p;
};

extern "C"
void *benchmarkGetInt(void *argsPtr) {
    BenchmarkCaseArgs& args = *(BenchmarkCaseArgs *)argsPtr;

    pthread_mutex_lock(args.d_mutex_p);
    if (args.d_numThreads == ++*args.d_numStarted_p) {
        pthread_cond_broadcast(args.d_cond_p);
        pthread_mutex_unlock(args.d_mutex_p);
    }
    else {
        while (*args.d_numStarted_p != args.d_numThreads) {
            pthread_cond_wait(args.d_cond_p, args.d_mutex_p);
        }
        pthread_mutex_unlock(args.d_mutex_p);
    }

    bdet_TimeInterval start = bdetu_SystemTime::now();
    for (int i = 0; i < args.d_numIterations; ++i) {
        Obj::getInt(*args.d_int_p);
    }
    bdet_TimeInterval end = bdetu_SystemTime::now();
    Obj::addInt(args.d_total_p, (end - start).totalMilliseconds());

    return 0;
}

extern "C"
void *benchmarkTestAndSwapInt(void *argsPtr) {
    BenchmarkCaseArgs& args = *(BenchmarkCaseArgs *)argsPtr;

    pthread_mutex_lock(args.d_mutex_p);
    if (args.d_numThreads == ++*args.d_numStarted_p) {
        pthread_cond_broadcast(args.d_cond_p);
        pthread_mutex_unlock(args.d_mutex_p);
    }
    else {
        while (*args.d_numStarted_p != args.d_numThreads) {
            pthread_cond_wait(args.d_cond_p, args.d_mutex_p);
        }
        pthread_mutex_unlock(args.d_mutex_p);
    }

    bdet_TimeInterval start = bdetu_SystemTime::now();
    for (int i = 0; i < args.d_numIterations; ++i) {
        Obj::testAndSwapInt(args.d_int_p, 0, 0);
    }
    bdet_TimeInterval end = bdetu_SystemTime::now();
    Obj::addInt(args.d_total_p, (end - start).totalMilliseconds());

    return 0;
}

#endif

//=============================================================================
//                       USAGE EXAMPLE FROM HEADER
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifndef BSLS_PLATFORM__CPU_POWERPC
    // Return immediately on unsupported platforms, avoiding warnings from
    // code analysis tools.
    return -1;
#else

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BSLS_PLATFORM__OS_AIX
    pthread_setconcurrency(20);
#endif

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // TESTING relaxed increment
        //
        // Create N threads each of which invokes addIntNvRelaxed(x,1) M
        // times.  Verify that the final value is NxM, and that each
        // return value is in the range (x, NxM] where x is the prior value.
        // --------------------------------------------------------------------
        enum {
            N = 10,
            M = 80000
        };

        Obj::Int value;
        Obj::initInt(&value, 0);

        Case8 args;
        args.d_value_p = &value;
        args.d_n = N;
        args.d_m = M;

        my_thread_t threadHandles[N];
        for (int i = 0; i < N; ++i) {
            myCreateThread(&threadHandles[i],
                           case8Thread,
                           &args);
        }

        for (int i = 0; i < N; ++i) {
            myJoinThread(threadHandles[i]);
        }

        ASSERT(N * M == Obj::getInt(value));
      } break;

      case 7: {
#ifdef BSLS_PLATFORM__OS_AIX
        // --------------------------------------------------------------------
        // TESTING getInt64 thread-safety with testAndSwapInt64
        //
        // Create many threads in two groups - one that invokes getInt64,
        // and one that invokes testAndSwapInt64.  The 'get' threads will
        // verify that the value they see is one of a set of pseudorandom
        // 64-bit values that the 'testAndSwap' threads are allowed to use.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nStress-test getInt64 with testAndSwapInt64"
                          << "\n=========================================="
                          << endl;
        {
            enum {
                NUM_GET_THREADS = 20,
                NUM_UPDATE_THREADS = 8,
                NUM_VALUES = 12000,
                TEST_DURATION = 12  //seconds
            };
            pthread_setconcurrency(NUM_GET_THREADS + NUM_UPDATE_THREADS);

            srandom(12345678);

            StressTestArgs args;
            // Initialize values
            for (int i = 0; i < NUM_VALUES; ++i) {
                bsls_PlatformUtil::Int64 x =
                    random() |
                       ((((bsls_PlatformUtil::Int64)random()) << 32)
                                                       & 0xffffffff00000000LL);
                bsl::vector<bsls_PlatformUtil::Int64>::iterator pos =
                    bsl::lower_bound(args.d_values.begin(),
                                     args.d_values.end(), x);
                args.d_values.insert(pos, x);
            }

            if (veryVerbose) {
                cout << "Setting initial value at "
                     << &args.d_value
                     << " to " << args.d_values[0]
                     << endl;
            }
            Obj::setInt64(&args.d_value, args.d_values[0]);
            if (veryVerbose) {
                cout << "Done setting initial value" << endl;
            }

            Obj::setInt(&args.d_done, 0);
            args.d_numValues = NUM_VALUES;

            my_thread_t threadHandles[NUM_GET_THREADS + NUM_UPDATE_THREADS];
            int threadIndex = 0;

            // Create reader ('getInt64') threads
            for (int i = 0; i < NUM_GET_THREADS; ++i) {
                myCreateThread(&threadHandles[threadIndex++],
                               stressTestReaderThread,
                               &args);
            }

            // Create update ('testAndSetInt64') threads
            for (int i = 0; i < NUM_UPDATE_THREADS; ++i) {
                myCreateThread(&threadHandles[threadIndex++],
                               stressTestUpdateThread,
                               &args);
            }

            sleep(2); // wait for all threads to wait...simple

            args.d_barrier.signal();

            sleep(TEST_DURATION);

            if (veryVerbose) {
                cout << "Test run for " << TEST_DURATION
                     << " seconds, shutting down threads..."
                     << endl;
            }
            Obj::setInt(&args.d_done, 1);
            while (threadIndex > 0) {
                myJoinThread(threadHandles[--threadIndex]);
            }
        }
        break;
      }
#endif

      case 6: {
        // --------------------------------------------------------------------
        // TESTING Spinlocks
        //   Test basic spinlock behavior assert that the lock
        //   behaves properly when concurrently accessed by multiple threads.
        // Plan:
        //   Begin by performing basic non-threaded tests to assert that the
        //   lock, unlock, and trylock operations are working correctly.  Next
        //   create a series of threads that will each acquire the same spin
        //   lock, increment a counter and release the spin lock for specific
        //   number of iterations.  If the lock is behaving as expected, the
        //   final value of the counter should be the number of threads *
        //   iterations.
        // Testing:
        //   initSpinLock(bces_AtomicUtilImpl...::SpinLock *aSpin);
        //   spinLock(bces_AtomicUtilImpl...::SpinLock *aSpin);
        //   spinTryLock(bces_AtomicUtilImpl...::SpinLock *aSpin, int retries);
        //   spinUnlock(bces_AtomicUtilImpl...::SpinLock *aSpin);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Spinlocks"
                          << "\n================="
                          << endl;
        {
            int result;
            Obj::SpinLock mS1;
            Obj::initSpinLock(&mS1);

            Obj::spinLock(&mS1);

            result = Obj::spinTryLock(&mS1,100);
            ASSERT(0 != result);

            Obj::spinUnlock(&mS1);

            result = Obj::spinTryLock(&mS1,1);
            ASSERT(0 == result);
            result = Obj::spinTryLock(&mS1,1);
            ASSERT(0 != result);

            Obj::spinUnlock(&mS1);
        }
        if (verbose) cout << "\nTesting Spinlocks Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS * NITERATIONS
            };

            Obj::SpinLock mSpin;

            Obj::initSpinLock(&mSpin);
            SpinLockTestThreadArgs args;

            args.d_lock_p = &mSpin;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_count  = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],spinLockTestThread, &args);
                args.d_startSig.wait();
            }

            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == args.d_count);
            if (veryVerbose) {
                T_(); P_(args.d_count) P(EXPTOTAL);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING INCREMENT/DECREMENT MANIPULATORS
        //   Test the atomic increment and decrement functions for the Int and
        //   Int64 atomic types.
        // Plan:
        //   For each atomic type(Int,Int64), using a sequence of
        //   independent values, begin by initializing the atomic Int/Int64 to
        //   a base value and increment the value by 1.  Next assert
        //   that the value is the expected value.  Then repeat the
        //   same operation using the "Nv" operation.  Assert that the
        //   value is the expected value, and that the returned value
        //   is also the expected value.  Finally create a series of
        //   threads to concurrently increment/decrement the same
        //   atomic Int/Int64 value for a specific number of
        //   iterations.  If the final value is the expected value,
        //   then it is assumed that the operations behave correctly
        //   in a multi-threaded environment.
        //
        // Testing:
        //   incrementInt(bces_AtomicUtilImpl...::Int *aInt);
        //   decrementInt(bces_AtomicUtilImpl...::Int *aInt);
        //   incrementIntNv(bces_AtomicUtilImpl...::Int *aInt);
        //   decrementIntNv(bces_AtomicUtilImpl...::Int *aInt);
        //   incrementInt64(bces_AtomicUtilImpl...::Int64 *aInt);
        //   decrementInt64(bces_AtomicUtilImpl...::Int64 *aInt);
        //   incrementInt64Nv(bces_AtomicUtilImpl...::Int64 *);
        //   decrementInt64Nv(bces_AtomicUtilImpl...::Int64 *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Increment/Decrement Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
                int d_expected;    // Expected resulting value
            } VALUES[] = {
                //line value expected
                //---- ----- --------
                { L_,   0   , 1       },
                { L_,   1   , 2       },
                { L_,  -1   , 0       },
                { L_,   2   , 3       },
                { L_,  -2   , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
                Obj::incrementInt(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
                result = Obj::incrementIntNv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS * NITERATIONS * 2,
                STARTVALUE  = 0
            };

            Obj::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' Decrement Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_expected;     // Expected result of decrement
                int d_value;        // Base value to be decremented
            } VALUES[] = {
                //line expected value
                //---- -------- --------
                { L_,   0     , 1       },
                { L_,   1     , 2       },
                { L_,  -1     , 0       },
                { L_,   2     , 3       },
                { L_,  -2     , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
                Obj::decrementInt(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
                result = Obj::decrementIntNv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS * NITERATIONS * 2,
                STARTVALUE  = 0
            };

            Obj::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' decrement Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = 33,
                STARTVALUE  = (NTHREADS*NITERATIONS*2)+EXPTOTAL
            };

            Obj::Int mInt;

            IntTestThreadArgs args;
            Obj::initInt(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], decrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls_PlatformUtil::Int64 d_expected; // Expected resulting
                                                     // value of decrement
                bsls_PlatformUtil::Int64 d_value; // Input value

            } VALUES[] = {
                //line expected             value
                //---- -------------------  ---------------------
                { L_,   0                  , 1                    },
                { L_,   1                  , 2                    },
                { L_,  -1LL                , 0                    },
                { L_,   0xFFFFFFFFLL       , 0x100000000LL        },
                { L_,  0xFFFFFFFFFFFFFFFFLL , 0                   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL  = VALUES[i].d_value;
                const bsls_PlatformUtil::Int64 EXP  = VALUES[i].d_expected;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(X));
                Obj::decrementInt64(&x);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL  = VALUES[i].d_value;
                const bsls_PlatformUtil::Int64 EXP  = VALUES[i].d_expected;
                bsls_PlatformUtil::Int64       result;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(x)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(X));
                result = Obj::decrementInt64Nv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Increment Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000
            };

            const bsls_PlatformUtil::Int64 STARTVALUE=0xfffffff0LL;
            const bsls_PlatformUtil::Int64 EXPTOTAL=NTHREADS*NITERATIONS*2+
                                           STARTVALUE;
            Obj::Int64 mInt;

            Obj::initInt64(&mInt,STARTVALUE);
            Int64TestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], incrementInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' decrement Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000
            };

            const bsls_PlatformUtil::Int64 EXPTOTAL=0xfffffff0;
            const bsls_PlatformUtil::Int64 STARTVALUE=(NTHREADS*NITERATIONS*2)+
                                           EXPTOTAL;
            Obj::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], decrementInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING SWAP MANIPULATORS:
        //   Test the swap and test and swap functions for the Int,Int64, and
        //   Pointer atomic types.
        //
        // Plan:
        //   For each atomic type("Int", "Int64", "Pointer"), perform the
        //   following tests to test the swap, and testAndSwap manipulators.
        //
        // 1 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'swap' it with a second test
        //   value and assert that the new value is the swapped value and that
        //   the return value is the base value.  Then create a series of
        //   threads, each of which will concurrently swap the same object with
        //   a predefined test value and increment counters indicating the
        //   number of times a particular value was seen.  A third thread will
        //   concurrently swap the object to second test value and also
        //   increment counters indicating the number of times a particular
        //   value was seen.  Finally assert that final counts equal the total
        //   number of times each value should have been seen.
        //
        // 2 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'testAndSwap' it with a
        //   second test value and assert that the new value is the expected
        //   value and that the return value is the expected return value.
        //   Then create a series of threads, each of which will concurrently
        //   test and swap the same object with a predefined test value and
        //   increment counters indicating the number of times a particular
        //   value was seen.  A third thread will concurrently swap the object
        //   to second test value and also increment counters indicating the
        //   number of times a particular value was seen.  Finally assert that
        //   final counts equal the total number of times each value should
        //   have been seen.
        //
        // Testing:
        //   swapInt64(bces_AtomicUtilImpl...::Int *aInt, int value);
        //   testAndSwapInt(bces_AtomicUtilImpl...::Int *, int, int);
        //   swapInt64(bces_AtomicUtilImpl...::Int64 *,
        //             bsls_PlatformUtil::Int64);
        //   testAndSwapInt64(bces_AtomicUtilImpl...::Int64 *,
        //                    bsls_PlatformUtil::Int6
        //                    bsls_PlatformUtil::Int64 );
        //   swapPtr(bces_AtomicUtilImpl...::Pointer *aPointer, void *value);
        //   testAndSwapPtr(bces_AtomicUtilImpl...::Pointer *, void *, void *);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting SWAP Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0   , 11     },
                { L_,   1   , 19     },
                { L_,  -1   , 4      },
                { L_,   2   , -4     },
                { L_,  -2   , 16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int SWPVAL = VALUES[i].d_swapValue;
                int       result = 0;

                Obj::Int x; const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(x));

                Obj::setInt(&x,VAL);
                result = Obj::swapInt(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(x)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt(X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        if (verbose) cout << endl
                          << "\tTesting 'testAndSwapInt'" << endl
                          << "\t------------------------" << endl;
        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
                int d_cmpValue;    // Compare value
                int d_expValue;    // Expected value after the operations
                int d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0   , 11         , 33     , 0          , 0       },
                { L_,   1   , 19         , 1      , 19         , 1       },
                { L_,  -1   , 4          , 1      , -1         , -1      },
                { L_,   2   , 0xFFFFFFFF , 2      , 0xFFFFFFFF , 2       },
                { L_,  -2   , 16         , 0      , -2         , -2      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int CMPVAL = VALUES[i].d_cmpValue;
                const int SWPVAL = VALUES[i].d_swapValue;
                const int EXPVAL = VALUES[i].d_expValue;
                const int EXPRES = VALUES[i].d_expResult;
                int       result = 0;

                Obj::Int x; const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(x));

                Obj::setInt(&x,VAL);
                result = Obj::testAndSwapInt(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES); NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt(X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int64' SWAP Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls_PlatformUtil::Int64 d_value;       // Initial value
                bsls_PlatformUtil::Int64 d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0LL , 11LL     },
                { L_,   1LL , 19LL     },
                { L_,  -1LL ,  4LL     },
                { L_,   2LL , -4LL     },
                { L_,  -2LL , 16LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL    = VALUES[i].d_value;
                const bsls_PlatformUtil::Int64 SWPVAL = VALUES[i].d_swapValue;
                bsls_PlatformUtil::Int64       result = 0;

                Obj::Int64 x; const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,VAL);
                result = Obj::swapInt64(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P_(VAL);
                    P_(SWPVAL); NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt64(X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls_PlatformUtil::Int64 d_value;       // Initial value
                bsls_PlatformUtil::Int64 d_swapValue;   // Swap value
                bsls_PlatformUtil::Int64 d_cmpValue;    // Compare value
                bsls_PlatformUtil::Int64 d_expValue;    // Expected value
                                                        // after the operation
                bsls_PlatformUtil::Int64 d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0LL  , 11         , 33     , 0          , 0       },
                { L_,   1LL  , 19         , 1      , 19         , 1       },
                { L_,  -1LL  , 4          , 1      , -1LL        , -1LL     },
                { L_,   2LL  , 0xFFFFFFFFFLL, 2    , 0xFFFFFFFFFLL, 2       },
                { L_,  -2LL  , 16         , 0      , -2LL        , -2LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL    = VALUES[i].d_value;
                const bsls_PlatformUtil::Int64 CMPVAL = VALUES[i].d_cmpValue;
                const bsls_PlatformUtil::Int64 SWPVAL = VALUES[i].d_swapValue;
                const bsls_PlatformUtil::Int64 EXPVAL = VALUES[i].d_expValue;
                const bsls_PlatformUtil::Int64 EXPRES = VALUES[i].d_expResult;
                bsls_PlatformUtil::Int64       result = 0;

                Obj::Int64 x; const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,VAL);
                ASSERT(VAL == Obj::getInt64(X));
                result = Obj::testAndSwapInt64(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt64(X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }
        if (verbose) cout << "\nTesting 'Pointer' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   (void*)0   , (void*)11     },
                { L_,   (void*)1   , (void*)19     },
                { L_,  (void*)-1   , (void*)4      },
                { L_,  (void*) 2   , (void*)-4     },
                { L_,  (void*)-2   , (void*)16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const void *VAL    = VALUES[i].d_value;
                const void *SWPVAL = VALUES[i].d_swapValue;
                void       *result = 0;

                Obj::Pointer x;
                const Obj::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(X));

                Obj::setPtr(&x,VAL);
                result = Obj::swapPtr(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(X)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getPtr(X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int  d_lineNum;      // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
                void *d_cmpValue;    // Compare value
                void *d_expValue;    // Expected value after the operations
                void *d_expResult;   // Expected result
            } VALUES[] = {
                //line value        swapVal             cmpVal
                //---- ------------ ------------------- --------------
                //    expValue             expResult
                //    -------------------- ----------------
                { L_, (void*) 0   , (void*)11         , (void*)33     ,
                      (void*)0          , (void*)0       },
                { L_, (void*) 1   , (void*)19         , (void*)1      ,
                      (void*)19         , (void*)1       },
                { L_, (void*)-1   , (void*)4          , (void*)1      ,
                      (void*)-1         , (void*)-1      },
                { L_, (void*) 2   , (void*)0xFFFFFFFF , (void*)2      ,
                      (void*)0xFFFFFFFF , (void*)2       },
                { L_, (void*)-2   , (void*)16         , (void*)0      ,
                      (void*)-2         , (void*)-2      }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const void *VAL    = VALUES[i].d_value;
                const void *CMPVAL = VALUES[i].d_cmpValue;
                const void *SWPVAL = VALUES[i].d_swapValue;
                const void *EXPVAL = VALUES[i].d_expValue;
                const void *EXPRES = VALUES[i].d_expResult;
                void       *result = 0;

                Obj::Pointer x;
                const Obj::Pointer& X = x;
                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(X));

                Obj::setPtr(&x,VAL);
                result = Obj::testAndSwapPtr(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getPtr(X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int' swap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS + NITERATIONS
            };

            Obj::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapIntTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;

            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapInt( &mInt,INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }

            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' swap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS+NITERATIONS
            };

            Obj::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapInt64TestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls_PlatformUtil::Int64 oldValue =
                    Obj::swapInt64(&mInt, INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int' testAndSwap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS+NITERATIONS
            };

            Obj::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapIntTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapInt( &mInt, INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' testAndSwap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS+NITERATIONS
            };

            Obj::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapInt64TestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls_PlatformUtil::Int64 oldValue =
                    Obj::swapInt64( &mInt, INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' swap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS+NITERATIONS
            };

            Obj::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapPtrTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void *oldValue = Obj::swapPtr(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' testAndSwap Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                EXPTOTAL    = NTHREADS*NITERATIONS+NITERATIONS
            };

            Obj::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapPtrTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void *oldValue = Obj::swapPtr(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC MANIPULATORS
        //   Test that the 32/64 bit integer add functions work as expected.
        // Plan:
        //   For each atomic type('Int', and 'Int64') using a sequence of
        //   independent values, begin by initializing the value to 0
        //   and adding the test value.  Assert the the resulting
        //   value is the expected value.  Repeat the operation using
        //   the "Nv" operation and assert the both the resulting
        //   value of the object and the return value from the
        //   operation are both the expected value.  Next, initialize
        //   each object to a base value add the delta value.  Assert
        //   that the result are still correct.  Repeat this step for
        //   the "Nv" operation and assert that the value return value
        //   and resulting object values are correct.  Finally create
        //   a series of threads each of which will concurrently add a
        //   specific value to a common instance.  If the final value
        //   is the expected value, then the operations are assumed to
        //   behave correctly when concurrently accessed by multiple
        //   threads.
        // Testing:
        //   addInt(bces_AtomicUtilImpl...::Int *aInt, int value);
        //   addInt64(bces_AtomicUtilImpl...::Int64 *,
        //            bsls_PlatformUtil::Int64);
        //   addIntNv(bces_AtomicUtilImpl...::Int *aInt, int value);
        //   addInt64Nv(bces_AtomicUtilImpl...::Int64 *,
        //              bsls_PlatformUtil::Int64);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' Arithmetic Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::addInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
            }

            if (verbose) cout <<
                "\n\tTesting 'Int' Arithmetic(and values) Manipulators\n" <<
                "\n\t-------------------------------------------------"
                              << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                int       result;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                result = Obj::addIntNv(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
                LOOP_ASSERT(i, VAL == result);
            }

        }

        if (verbose) cout << "\n\tTesting 'Int' Arith(with base) Manip"
                          << endl;

        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_base;        // Base value
                int d_amount;      // Amount to add
                int d_expected;    // Expected value
            } VALUES[] = {
                //line d_base    d_amount d_expected
                //---- --------  -------- ----------
                { L_,   0       , -9    , -9         },
                { L_,   1       , 0     , 1          },
                { L_,  -1       , 1     , 0          },
                { L_, 0xFFFFFFFF, 1     , 0          },
                { L_,  -2       , -2    , -4         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(X));

                Obj::addInt(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X));
                    P_(BASE); P_(AMT); P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(X));

                result = Obj::addIntNv(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X));
                    P_(BASE); P_(AMT); P_(EXP); P_(result); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(X));
            }

        }

        if (verbose) cout << "\nTesting 'Int64' Arithmetic Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls_PlatformUtil::Int64 d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1LL   },
                { L_,   2   },
                { L_,  -2LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL  = VALUES[i].d_value;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::addInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL  = VALUES[i].d_value;
                bsls_PlatformUtil::Int64       result;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                result = Obj::addInt64Nv(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X));
                    P_(VAL); P_(result); NL();
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, VAL == Obj::getInt64(X));
            }

        }
        {
            static const struct {
                int  d_lineNum;      // Source line number
                bsls_PlatformUtil::Int64 d_base;    // Base value
                bsls_PlatformUtil::Int64 d_amount;  // Amount to add
                bsls_PlatformUtil::Int64 d_expected; // Expected value
            } VALUES[] = {
                //line d_base        d_amount d_expected
                //---- ------------- -------- ----------
                { L_,  -1LL         , 10      , 9                    },
                { L_,  1            , -2LL    , -1LL                 },
                { L_,  -1LL         , 2LL     , 1LL                  },
                { L_,  0xFFFFFFFFLL , 1LL    , 0x100000000LL        },
                { L_,  0x100000000LL, -2LL    , 0xFFFFFFFELL         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 BASE = VALUES[i].d_base;
                const bsls_PlatformUtil::Int64 AMT  = VALUES[i].d_amount;
                const bsls_PlatformUtil::Int64 EXP  = VALUES[i].d_expected;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(X));

                Obj::addInt64(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P(BASE);
                    T_(); P_(AMT); P(EXP);
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 BASE = VALUES[i].d_base;
                const bsls_PlatformUtil::Int64 AMT  = VALUES[i].d_amount;
                const bsls_PlatformUtil::Int64 EXP  = VALUES[i].d_expected;
                bsls_PlatformUtil::Int64       result;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(X));

                result = Obj::addInt64Nv(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P(BASE);
                    T_(); P_(AMT); P(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(X));
            }

        }
        if (verbose) cout << "\nTesting 'Int' add Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                ADDVAL      = 3,
                EXPTOTAL    = NTHREADS*NITERATIONS*ADDVAL*2,
                STARTVALUE  = 0
            };

            Obj::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],addIntTestThread, &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE); NL();
            }
        }
        if (verbose) cout << "\nTesting 'Int64' add Thread Safeness"
                          << endl;
        {
            enum {
                NTHREADS    = 4,
                NITERATIONS = 10000,
                ADDVAL      = 33
            };

            const bsls_PlatformUtil::Int64 STARTVALUE=0xfffff000;
            const bsls_PlatformUtil::Int64 EXPTOTAL=(NTHREADS*NITERATIONS*
                                                     ADDVAL * 2) + STARTVALUE;
            Obj::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);
            ASSERT(STARTVALUE == Obj::getInt64(mInt));

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], addInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        //
        // Plan:
        //   First, verify the initialization functions by initializing each
        //   atomic type and testing the resulting value.
        //
        //   Next, for the Int,Int64, and Pointer atomic types, for a sequence
        //   of independent test values, use the initialization functions to
        //   initialize each type and use the primary manipulator(i.e.,
        //   'setInt', 'setInt64', 'setPtr') to set its value.  Verify the
        //   value using the respective direct accessor(i.e.,'getInt',
        //   'getInt64', 'getPtr').
        //
        // Testing:
        //   initInt(bces_AtomicUtilImpl...::Int *aInt, int initialValue);
        //   initInt64(bces_AtomicUtilImpl...::Int64 *aInt);
        //   initPointer(bces_AtomicUtilImpl...::Pointer *aPointer);
        //   setInt(bces_AtomicUtilImpl...::Int *aInt, int value);
        //   getInt(const bces_AtomicUtilImpl...::Int &aInt);
        //   setInt64(bces_AtomicUtilImpl...::Int64 *,
        //            bsls_PlatformUtil::Int64);
        //   getInt64(const bces_AtomicUtilImpl...::Int64 &aInt);
        //   getPtr(const bces_AtomicUtilImpl...::Pointer &aPointer);
        //   setPtr(bces_AtomicUtilImpl...::Pointer *aPointer, void *value);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'Int' Primary Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Obj::Int x;  const Obj::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Primary Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls_PlatformUtil::Int64 d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- -----
                { L_,   0              },
                { L_,   1              },
                { L_,  -1              },
                { L_,  0xFFFFFFFFLL    },
                { L_,  0x100000000LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 VAL  = VALUES[i].d_value;

                Obj::Int64 x;  const Obj::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(X));
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' Primary Manipulators"
                           << endl;
        {
            static const struct {
                int    d_lineNum;     // Source line number
                void  *d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,  (void*)0x0        },
                { L_,  (void*)0x1        },
                { L_,  (void*)0xffff     },
                { L_,  (void*)0xffff8888 },
                { L_,  (void*)0xffffffff }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const void *VAL  = VALUES[i].d_value;

                Obj::Pointer x;
                const Obj::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(X));

                Obj::setPtr(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(X)); P_(VAL);
                }
                LOOP_ASSERT(i, VAL == Obj::getPtr(X));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

          if (verbose) cout << "\nBREATHING TEST"
                            << "\n==============" << endl;

        const int XVA=-1;
        const int XVB=2;
        const int XVC=-2;

        const bsls_PlatformUtil::Int64 YVA=1LL;
        const bsls_PlatformUtil::Int64 YVB=-1LL;
        const bsls_PlatformUtil::Int64 YVC=0xFFFFFFFFLL;

        const void *PVA=(void*)0xffff8888;
        const void *PVB=(void*)0xffffffff;
        const void *PVC=(void*)0x78888888;

        int lresult;
        bsls_PlatformUtil::Int64 llresult;
        void *presult;

        if (veryVerbose) cout << endl
                              << "\tInt" << endl
                              << "\t---" << endl;

        Obj::Int mX1;

        Obj::initInt(&mX1,0);
        ASSERT(0 == Obj::getInt(mX1));

        Obj::setInt(&mX1,XVA);
        ASSERT(XVA == Obj::getInt(mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(mX1));

        Obj::incrementInt(&mX1);
        ASSERT(1 == Obj::getInt(mX1));

        Obj::decrementInt(&mX1);
        ASSERT(0 == Obj::getInt(mX1));

        lresult = Obj::incrementIntNv(&mX1);
        ASSERT(1 == lresult);
        ASSERT(1 == Obj::getInt(mX1));
        if ( veryVeryVerbose) {
            T_(); P(lresult); NL();
        }
        lresult = Obj::decrementIntNv(&mX1);
        ASSERT(0 == lresult);
        ASSERT(0 == Obj::getInt(mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(mX1));

        lresult = Obj::addIntNv(&mX1,XVB);
        ASSERT(XVB == Obj::getInt(mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(mX1));

        Obj::setIntRelaxed(&mX1,5);
        ASSERT(5 == Obj::getIntRelaxed(mX1));

        lresult = Obj::swapInt(&mX1, XVC);
        ASSERT(5 == lresult);
        ASSERT(XVC == Obj::getInt(mX1));

        lresult = Obj::testAndSwapInt(&mX1,XVA,XVB);
        ASSERT(XVC == lresult);
        ASSERT(XVC == Obj::getInt(mX1));

        lresult = Obj::testAndSwapInt(&mX1,XVC,0);
        ASSERT(XVC == lresult);
        ASSERT(0 == Obj::getInt(mX1));

        if (veryVerbose) cout << endl
                              << "\tInt64" << endl
                              << "\t-----" << endl;

        Obj::Int64 mY1;

        Obj::initInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(mY1));

        Obj::setInt64(&mY1,YVA);
        LOOP2_ASSERT(YVA, Obj::getInt64(mY1), YVA == Obj::getInt64(mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(mY1));

        Obj::incrementInt64(&mY1);
        ASSERT(1 == Obj::getInt64(mY1));

        Obj::decrementInt64(&mY1);
        ASSERT(0 == Obj::getInt64(mY1));

        llresult = Obj::incrementInt64Nv(&mY1);
        ASSERT(1 == llresult);
        ASSERT(1 == Obj::getInt64(mY1));
        if ( veryVeryVerbose) {
            T_(); P(llresult); NL();
        }
        llresult = Obj::decrementInt64Nv(&mY1);
        ASSERT(0 == llresult);
        ASSERT(0 == Obj::getInt64(mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(mY1));

        llresult = Obj::addInt64Nv(&mY1,YVB);
        ASSERT(YVB == Obj::getInt64(mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(mY1));

        llresult = Obj::swapInt64(&mY1, YVC);
        ASSERT(0 == llresult);
        ASSERT(YVC == Obj::getInt64(mY1));

        llresult = Obj::testAndSwapInt64(&mY1,YVA,YVB);
        ASSERT(YVC == llresult);
        ASSERT(YVC == Obj::getInt64(mY1));

        llresult = Obj::testAndSwapInt64(&mY1,YVC,0);
        ASSERT(YVC == llresult);
        ASSERT(0 == Obj::getInt64(mY1));

        // Pointers
        // --------

        if (veryVerbose) cout << endl
                              << "\tPointers" << endl
                              << "\t--------" << endl;
        Obj::Pointer mP1;

        Obj::initPointer(&mP1,0);
        ASSERT(0 == Obj::getPtr(mP1));

        presult = Obj::swapPtr(&mP1, PVC);
        ASSERT(0 == presult);
        ASSERT(PVC == Obj::getPtr(mP1));

        presult = Obj::testAndSwapPtr(&mP1,PVA,PVB);
        ASSERT(PVC == presult);
        ASSERT(PVC == Obj::getPtr(mP1));

        presult = Obj::testAndSwapPtr(&mP1,PVC,0);
        ASSERT(PVC == presult);
        ASSERT(0 == Obj::getPtr(mP1));

        // Spin Locks
        // ----------

        if (veryVerbose) cout << endl
                              << "\tSpin Locks" << endl
                              << "\t----------" << endl;

        Obj::SpinLock mS1;
        Obj::initSpinLock(&mS1);

        Obj::spinLock(&mS1);

        lresult = Obj::spinTryLock(&mS1,100);
        ASSERT(0 != lresult);

        Obj::spinUnlock(&mS1);

        lresult = Obj::spinTryLock(&mS1,1);
        ASSERT(0 == lresult);

        Obj::spinUnlock(&mS1);
      } break;
      case -1: {
        // BENCHMARK

        {
            enum { ITERATIONS = 100 * 1000 * 1000, NUM_THREADS = 5 };
            Obj::Int mX;
            Obj::initInt(&mX,1);
            bdet_TimeInterval start = bdetu_SystemTime::now();
            int total = 0;
            for (int i = 0; i < ITERATIONS; ++i) {
                total += Obj::getInt(mX);
            }
            bdet_TimeInterval stop = bdetu_SystemTime::now();

            bsl::cout << "With one thread, "
                      << ITERATIONS
                      << " iterations of getInt took "
                      << (stop - start).totalMilliseconds()
                      << " ms."
                      << bsl::endl;
        }

        {
            enum { ITERATIONS = 100 * 1000 * 1000, NUM_THREADS = 5 };
            Obj::Int mX;
            Obj::initInt(&mX,1);
            bdet_TimeInterval start = bdetu_SystemTime::now();
            int total = 0;
            for (int i = 0; i < ITERATIONS; ++i) {
                total += Obj::testAndSwapInt(&mX, 0, 0);
            }
            bdet_TimeInterval stop = bdetu_SystemTime::now();

            bsl::cout << "With one thread, "
                      << ITERATIONS
                      << " iterations of testAndSwap took "
                      << (stop - start).totalMilliseconds()
                      << " ms."
                      << bsl::endl;
        }

        {
            enum { ITERATIONS =  10 * 1000 * 1000, NUM_THREADS = 5 };
            Obj::Int mX, total;
            Obj::initInt(&mX,1);
            Obj::initInt(&total,0);

            BenchmarkCaseArgs args;
            int numStarted = 0;
            pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
            pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

            args.d_int_p = &mX;
            args.d_total_p = &total;
            args.d_numIterations = ITERATIONS;
            args.d_numStarted_p = &numStarted;
            args.d_numThreads = NUM_THREADS;
            args.d_mutex_p = &mutex;
            args.d_cond_p = &cond;

            my_thread_t threadHandles[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                myCreateThread(&threadHandles[i],
                               &benchmarkGetInt,
                               &args);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }

            bsl::cout << "With "
                      << NUM_THREADS
                      << " threads, "
                      << ITERATIONS
                      << " iterations of getInt took "
                      << Obj::getInt(total)
                      << " ms."
                      << bsl::endl;
        }

        {
            enum { ITERATIONS =  10 * 1000 * 1000, NUM_THREADS = 5 };
            Obj::Int mX, total;
            Obj::initInt(&mX,1);
            Obj::initInt(&total,0);

            BenchmarkCaseArgs args;
            int numStarted = 0;
            pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
            pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

            args.d_int_p = &mX;
            args.d_total_p = &total;
            args.d_numIterations = ITERATIONS;
            args.d_numStarted_p = &numStarted;
            args.d_numThreads = NUM_THREADS;
            args.d_mutex_p = &mutex;
            args.d_cond_p = &cond;

            my_thread_t threadHandles[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                myCreateThread(&threadHandles[i],
                               &benchmarkTestAndSwapInt,
                               &args);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }

            bsl::cout << "With "
                      << NUM_THREADS
                      << " threads, "
                      << ITERATIONS
                      << " iterations of testAndSwapInt took "
                      << Obj::getInt(total)
                      << " ms."
                      << bsl::endl;
        }

      } break;
      default:
          bsl::cout << "Case " << test << " NOT FOUND" << bsl::endl;
        testStatus = -1;
    }
    return testStatus;
#endif
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
