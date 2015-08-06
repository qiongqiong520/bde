// bdlqq_mutex.t.cpp                                                  -*-C++-*-

#include <bdlqq_mutex.h>

#include <bdlqq_threadattributes.h>
#include <bdlqq_threadutil.h>

#include <bsl_iostream.h>
#include <bsl_map.h>

#include <bsls_atomic.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o line feed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bdlqq::Mutex Obj;

class ZeroInt {
    int d_i;

  public:
    ZeroInt() : d_i(0) {}
    ZeroInt& operator=(int i) { d_i = i; return *this; }
    ZeroInt& operator++() { ++d_i; return *this; }
    operator int() const { return d_i; }
};

struct ZeroIntMap : bsl::map<int, ZeroInt> {
    int sum() const
    {
        const const_iterator b = begin();
        const const_iterator e = end();

        int ret = 0;
        for (const_iterator it = b; e != it; ++it) {
            ret += it->second;
        }

        return ret;
    }
};

bsl::ostream& operator<<(bsl::ostream& stream, const ZeroIntMap& thismap)
{
    const ZeroIntMap::const_iterator b = thismap.begin();
    const ZeroIntMap::const_iterator e = thismap.end();

    stream << "Sum:" << thismap.sum();

    bool first_time = true;
    for (ZeroIntMap::const_iterator it = b; e != it; ++it) {
        if (first_time) {
            first_time = false;
        }
        else {
            stream << ',';
        }
        stream << ' ';
        stream << it->first << ':' << (int) it->second;
    }

    return stream;
}

                                // ------
                                // case 1
                                // ------
struct ThreadInfo {
    Obj *d_lock;
    bsls::AtomicInt d_retval;
    bsls::AtomicInt d_retvalSet;
};

extern "C" void* MyThread(void* arg_p)
{
    ThreadInfo* arg = (ThreadInfo*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

                                // -------
                                // case -1
                                // -------

namespace BCEMT_MUTEX_CASE_MINUS_1 {


    enum { NUM_NOT_URGENT_THREADS = 128,
           NUM_THREADS            = NUM_NOT_URGENT_THREADS + 1 };

int translatePriority(bdlqq::ThreadAttributes::SchedulingPolicy policy,
                      bool                                     low)
{
    if (low) {
        return bdlqq::ThreadUtil::getMinSchedulingPriority(policy);   // RETURN
    }
    else {
        return bdlqq::ThreadUtil::getMaxSchedulingPriority(policy);   // RETURN
    }
}

struct F {
    bool                  d_urgent;
    static int            s_urgentPlace;
    static bool           s_firstThread;
    static bsls::AtomicInt s_lockCount;
    static bsls::AtomicInt s_finished;
    static bdlqq::Mutex    s_mutex;

    // CREATORS
    F() : d_urgent(false) {}

    // ACCESSORS
    void operator()();
};
int            F::s_urgentPlace;
bool           F::s_firstThread = 1;
bsls::AtomicInt F::s_finished(0);
bsls::AtomicInt F::s_lockCount(0);
bdlqq::Mutex    F::s_mutex;

void F::operator()()
{
    enum { LIMIT = 10 * 1024 };

    for (int i = 0; i < LIMIT; ++i) {
        ++s_lockCount;
        s_mutex.lock();
        if (s_firstThread) {
            s_firstThread = false;

            // Careful!  This could take 2 seconds to wake up!

            bdlqq::ThreadUtil::microSleep(200 * 1000);
            ASSERT(NUM_THREADS == s_lockCount);
        }
        s_mutex.unlock();
        --s_lockCount;
    }

    if (d_urgent) {
        s_urgentPlace = s_finished;
    }
    ++s_finished;
}

}  // close namespace BCEMT_MUTEX_CASE_MINUS_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a mutex.  Lock and verify that tryLock fails;
        // unlock and verify that tryLock succeeds.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }
        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        {
            Obj lock;
            lock.lock();

            ThreadInfo args;
            args.d_lock = &lock;

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bdlqq::ThreadAttributes attr;
            attr.setDetachedState(
                               bdlqq::ThreadAttributes::BCEMT_CREATE_DETACHED);
            bdlqq::ThreadUtil::Handle dum;
            bdlqq::ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                bdlqq::ThreadUtil::microSleep(1000 * SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bdlqq::ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                bdlqq::ThreadUtil::microSleep(1000 * SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // should succeed
            if (verbose) {
                P(args.d_retval);
            }

        }
      } break;
      case -1: {
        // ------------------------------------------------------------------
        // Testing prioirities on heavily mutexed tasks
        // ------------------------------------------------------------------

        namespace TC = BCEMT_MUTEX_CASE_MINUS_1;

        typedef bdlqq::ThreadAttributes::SchedulingPolicy Policy;
        const Policy DF = bdlqq::ThreadAttributes::BCEMT_SCHED_DEFAULT;
        const Policy SO = bdlqq::ThreadAttributes::BCEMT_SCHED_OTHER;
        const Policy SF = bdlqq::ThreadAttributes::BCEMT_SCHED_FIFO;
        const Policy SR = bdlqq::ThreadAttributes::BCEMT_SCHED_RR;

        ZeroIntMap urgentPlaces[2];

        struct {
            int    d_line;
            Policy d_policy;
            bool   d_urgentLow;
            bool   d_normalizedPriorities;
        } DATA[] = {
            { L_, DF, 0, 0 },
            { L_, DF, 0, 1 },
            { L_, DF, 1, 0 },
            { L_, DF, 1, 1 },
            { L_, SO, 0, 0 },
            { L_, SO, 0, 1 },
            { L_, SO, 1, 0 },
            { L_, SO, 1, 1 },
            { L_, SF, 0, 0 },
            { L_, SF, 0, 1 },
            { L_, SF, 1, 0 },
            { L_, SF, 1, 1 },
            { L_, SR, 0, 0 },
            { L_, SR, 0, 1 },
            { L_, SR, 1, 0 },
            { L_, SR, 1, 1 },
        };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

        for (int i = 0; i < DATA_LEN; ++i) {
            const int    LINE       = DATA[i].d_line;
            const Policy POLICY     = DATA[i].d_policy;
            const int    URGENT_LOW = DATA[i].d_urgentLow;
            const int    NORM_PRI   = DATA[i].d_normalizedPriorities;

            const int    URGENT_PRIORITY =     TC::translatePriority(
                                                                  POLICY,
                                                                  URGENT_LOW);
            const int    NOT_URGENT_PRIORITY = TC::translatePriority(
                                                                  POLICY,
                                                                  !URGENT_LOW);

            const double NORM_URGENT_PRI     = URGENT_LOW ? 0.0 : 1.0;
            const double NORM_NOT_URGENT_PRI = URGENT_LOW ? 1.0 : 0.0;

            if (veryVerbose) {
                if (NORM_PRI) {
                    P_(LINE) P_(POLICY) P(NORM_URGENT_PRI);
                }
                else {
                    P_(LINE) P_(POLICY)
                    P_(URGENT_PRIORITY) P(NOT_URGENT_PRIORITY);
                }
            }

            ASSERT(URGENT_PRIORITY != NOT_URGENT_PRIORITY);

            TC::F::s_urgentPlace = -1;
            TC::F::s_finished = 0;
            TC::F::s_firstThread = true;

            TC::F fs[TC::NUM_THREADS];
            bdlqq::ThreadUtil::Handle handles[TC::NUM_THREADS];

            bdlqq::ThreadAttributes notUrgentAttr;
            notUrgentAttr.setStackSize(1024 * 1024);
            notUrgentAttr.setInheritSchedule(0);
            notUrgentAttr.setSchedulingPolicy(POLICY);

            bdlqq::ThreadAttributes urgentAttr(notUrgentAttr);

            if (NORM_PRI) {
                notUrgentAttr.setSchedulingPriority(
                                bdlqq::ThreadUtil::convertToSchedulingPriority(
                                                 POLICY, NORM_NOT_URGENT_PRI));
                urgentAttr.   setSchedulingPriority(
                                bdlqq::ThreadUtil::convertToSchedulingPriority(
                                                 POLICY, NORM_URGENT_PRI));
            }
            else {
                notUrgentAttr.setSchedulingPriority(NOT_URGENT_PRIORITY);
                urgentAttr.   setSchedulingPriority(    URGENT_PRIORITY);
            }

            fs[TC::NUM_THREADS - 1].d_urgent = true;

            int rc;
            int numThreads = 0;
            for ( ; numThreads < TC::NUM_THREADS; ++numThreads) {
                bdlqq::ThreadAttributes *attr
                                      = numThreads < TC::NUM_NOT_URGENT_THREADS
                                      ? &notUrgentAttr
                                      : &urgentAttr;
                rc = bdlqq::ThreadUtil::create(&handles[numThreads],
                                              *attr,
                                              fs[numThreads]);
                LOOP3_ASSERT(LINE, rc, numThreads, 0 == rc);
                if (rc) {
                    break;
                }
            }

            for (int j = 0; j < numThreads; ++j) {
                rc = bdlqq::ThreadUtil::join(handles[j]);
                LOOP3_ASSERT(LINE, rc, j, 0 == rc);
                if (rc) {
                    break;
                }
            }

            ASSERT(TC::F::s_urgentPlace >= 0);
            ASSERT(TC::F::s_urgentPlace < TC::NUM_THREADS);
            ASSERT(!TC::F::s_firstThread);
            ASSERT(TC::NUM_THREADS == TC::F::s_finished);

            urgentPlaces[URGENT_LOW][LINE] = TC::F::s_urgentPlace;
        }

        cout << "Urgent low:  " << urgentPlaces[true ] << endl;
        cout << "Urgent high: " << urgentPlaces[false] << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
