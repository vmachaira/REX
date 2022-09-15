#ifndef GENERAL_MACROS_H
#define GENERAL_MACROS_H

    #define DO_PRAGMA(x) _Pragma (#x)
    #define TODO(x) DO_PRAGMA(message ("[TODO] " #x))
    #define OPTFLAG(x) DO_PRAGMA(message ("[OPTION] " #x))
    #define OPTVALUE(x) DO_PRAGMA(message ("[OPTION] " #x "=" Stringize(x)))
    //example:
    //TODO(Remember to fix this)

    #define Stringize( L )     #L
    #define StringValue( L ) Stringize( L )
    #define MakeString( M, L ) M(L)
    #define $Line MakeString( Stringize, __LINE__ )
    #define CODEINFO(x) DO_PRAGMA(message (__FILE__ "(" $Line "):" #x))
    #define MYPRINT(x) std::cout << #x"=" << x << std::flush << std::endl;
    #define MYWRITE(x) \
    {std::ofstream debugFile( "debug_log.txt", std::fstream::out | std::fstream::app); \
     debugFile << #x"=" << x << std::endl << std::flush;};
    #define TIC { Timer timer; timer.start();
    #define TOC timer.stop(); std::cout << "elapsed time:" << timer.getElapsedTime() << "\n"; }

    //preprocessor directives
    #if !defined(NUM_THREADS)
        #define NUM_THREADS 1
    #endif

    #if NUM_THREADS > 1
        #include <pthread.h>
        pthread_mutex_t mutexPrint = PTHREAD_MUTEX_INITIALIZER;
    #endif

    #if defined(ENABLE_INFO_PRINT)
        #if NUM_THREADS > 1
            #define oILOG(text) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                std::cout << "[thread-" << pthread_self() << "]>" << text << std::flush; \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define oILOG(text) \
                { \
                std::cout << text << std::flush; \
                }
        #endif
    #else
        #define oILOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_DEBUG_PRINT)
        #if NUM_THREADS > 1
            #define oDLOG(text) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                std::cout << "[thread-" << pthread_self() << "]>" << text << std::flush; \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define oDLOG(text) \
                { \
                std::cout << text << std::flush; \
                }
        #endif
    #else
        #define oDLOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_VERBOSE_PRINT)
        #if NUM_THREADS > 1
            #define VERBOSE(text) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                std::cout << "[thread-" << pthread_self() << "]>" << text << std::flush; \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define VERBOSE(text) \
                { \
                std::cout << text << std::flush; \
                }
        #endif
    #else
        #define VERBOSE(fmt, ...) {;}
    #endif

    #if defined(ENABLE_WARN_PRINT)
        #if NUM_THREADS > 1
            #define oWLOG(text) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                std::cout << "[thread-" << pthread_self() << "]>" << text << std::flush; \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define oWLOG(text) \
                { \
                std::cout << text << std::flush; \
                }
        #endif
    #else
        #define oWLOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_ERROR_PRINT)
        #if NUM_THREADS > 1
            #define oELOG(text) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                std::cout << "[thread-" << pthread_self() << "]>" << text << std::flush; \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define oELOG(text) \
                { \
                std::cout << text << std::flush; \
                }
        #endif
    #else
        #define oELOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_INFO_PRINT)
        #if NUM_THREADS > 1
            #define ILOG(fmt, ...) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                printf("[thread-%ld]>", pthread_self());\
                printf(fmt, ##__VA_ARGS__); \
                fflush(stdout); \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define ILOG(fmt, ...) \
                { \
                printf(fmt, ##__VA_ARGS__); \
                fflush(stdout); \
                }
        #endif
    #else
        #define ILOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_DEBUG_PRINT)
        #if NUM_THREADS > 1
            #define DLOG(fmt, ...) \
                { \
                pthread_mutex_lock(&mutexPrint); \
                printf("[thread-%ld]>", pthread_self()); \
                printf(fmt, ##__VA_ARGS__); \
                fflush(stdout); \
                pthread_mutex_unlock(&mutexPrint); \
                }
        #else
            #define DLOG(fmt, ...) \
                { \
                printf(fmt, ##__VA_ARGS__); \
                fflush(stdout); \
                }
        #endif
    #else
        #define DLOG(fmt, ...) {;}
    #endif

    #if defined(ENABLE_DEBUG_CODE)
        #define DBG(code) { code }
    #else
        #define DBG(code) {;}
    #endif

    //graph structures => ADJ:adjacency list, DnFS:Dynamic Forward Star, PMG:Packed-Memory Graph, StPMG: Similar to Static Forward Star
    #if !defined(GraphImplType)
        #if defined(PM_GRAPH_IMPL)
            #define GraphImplType PackedMemoryArrayImpl
        #elif defined(S_FS_GRAPH_IMPL)
            #define GraphImplType StaticForwardStarImpl
        #elif defined(D_FS_GRAPH_IMPL)
            #define GraphImplType ForwardStarImpl
        #else //defined(AL_GRAPH_IMPL)
            #define GraphImplType AdjacencyListImpl
        #endif
    #endif

    #define HALT { int a; std::cin>>a;}

#endif

