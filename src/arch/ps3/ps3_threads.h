#pragma once

#include <sys/queue.h>

// #define PS3_LW_PRIMITIVES


#include <sys/thread.h>

/**
 * Mutexes
 */
// #define PS3_DEBUG_MUTEX


#ifndef PS3_DEBUG_MUTEX

#ifdef PS3_LW_PRIMITIVES
typedef sys_lwmutex_t hts_mutex_t;
#else
typedef sys_mutex_t hts_mutex_t;
#endif

extern void hts_mutex_init(hts_mutex_t *m);
extern void hts_mutex_init_recursive(hts_mutex_t *m);
extern void hts_mutex_lock(hts_mutex_t *m);
extern void hts_mutex_unlock(hts_mutex_t *m);
extern void hts_mutex_destroy(hts_mutex_t *m);
#define hts_mutex_assert(m)

#else

typedef struct {
#ifdef PS3_LW_PRIMITIVES
  sys_lwmutex_t mtx;
#else
  sys_mutex_t mtx;
#endif
  int dbg;
  int logptr;
  struct {
    enum {
      MTX_CREATE = 1,
      MTX_LOCK,
      MTX_UNLOCK,
      MTX_DESTROY,
      MTX_SAMPLE,
    } op;
    const char *file;
    int line;
    int thread;
#ifdef PS3_LW_PRIMITIVES
    uint64_t lock_var;
#endif
  } log[16];
} hts_mutex_t;

extern void hts_mutex_initx(hts_mutex_t *m, const char *file, int line, int dbg);
extern void hts_mutex_initx_recursive(hts_mutex_t *m, const char *file, int line);

extern void hts_mutex_lockx(hts_mutex_t *m, const char *file, int line);
extern void hts_mutex_unlockx(hts_mutex_t *m, const char *file, int line);
extern void hts_mutex_destroyx(hts_mutex_t *m, const char *file, int line);

#define hts_mutex_init(m) hts_mutex_initx(m, __FILE__, __LINE__, 0)
#define hts_mutex_init_recursive(m) hts_mutex_initx_recursive(m, __FILE__, __LINE__)
#define hts_mutex_dbg(m) hts_mutex_initx(m, __FILE__, __LINE__, 1)
#define hts_mutex_lock(m) hts_mutex_lockx(m, __FILE__, __LINE__)
#define hts_mutex_unlock(m) hts_mutex_unlockx(m, __FILE__, __LINE__)
#define hts_mutex_destroy(m) hts_mutex_destroyx(m, __FILE__, __LINE__)

#ifdef PS3_LW_PRIMITIVES
#define hts_mutex_assert(m) assert((m)->mtx.lock_var != 0xffffffff00000000LL);
#else
#define hts_mutex_assert(m)
#endif

#endif

/**
 * Condition variables
 */
// #define PS3_DEBUG_COND
#ifdef PS3_LW_PRIMITIVES
typedef sys_lwcond_t hts_cond_t;
#else
typedef sys_cond_t hts_cond_t;
#endif

#ifndef PS3_DEBUG_COND

extern void hts_cond_init(hts_cond_t *c, hts_mutex_t *m);
extern void hts_cond_destroy(hts_cond_t *c);
extern void hts_cond_signal(hts_cond_t *c);
extern void hts_cond_broadcast(hts_cond_t *c);
#define hts_cond_wait(c, m) hts_cond_wait_timeout(c, m, 0)
extern int hts_cond_wait_timeout(hts_cond_t *c, hts_mutex_t *m, int delay);


#else // condvar debugging

extern void hts_cond_initx(hts_cond_t *c, hts_mutex_t *m, const char *file, int line);
extern void hts_cond_destroyx(hts_cond_t *c, const char *file, int line);
extern void hts_cond_signalx(hts_cond_t *c, const char *file, int line);
extern void hts_cond_broadcastx(hts_cond_t *c, const char *file, int line);
extern void hts_cond_waitx(hts_cond_t *c, hts_mutex_t *m, const char *file, int line);
extern int hts_cond_wait_timeoutx(hts_cond_t *c, hts_mutex_t *m, int delay, const char *file, int line);

#define hts_cond_init(c, m) hts_cond_initx(c, m, __FILE__, __LINE__)
#define hts_cond_destroy(c) hts_cond_destroyx(c, __FILE__, __LINE__)
#define hts_cond_signal(c)     hts_cond_signalx(c, __FILE__, __LINE__)
#define hts_cond_broadcast(c)  hts_cond_broadcastx(c, __FILE__, __LINE__)
#define hts_cond_wait(c, m) hts_cond_waitx(c, m,  __FILE__, __LINE__)
#define hts_cond_wait_timeout(c, m, d) hts_cond_wait_timeoutx(c, m, d, __FILE__, __LINE__) 

#endif


/**
 * Threads
 */
typedef sys_ppu_thread_t hts_thread_t;

#define THREAD_PRIO_LOW    3000
#define THREAD_PRIO_NORMAL 2999
#define THREAD_PRIO_HIGH   3

extern void hts_thread_create_detached(const char *, void *(*)(void *), void *,
				       int prio);

extern void hts_thread_create_joinable(const char *, hts_thread_t *p,
				       void *(*)(void *), void *,
				       int prio);

extern void hts_thread_join(hts_thread_t *id);

#define hts_thread_detach(t) sys_ppu_thread_detach(*(t))

extern hts_thread_t hts_thread_current(void);



LIST_HEAD(thread_info_list, thread_info);
typedef struct thread_info {
  LIST_ENTRY(thread_info) link;
  void *aux;
  void *(*fn)(void *);
  sys_ppu_thread_t id;
  char name[64];
} thread_info_t;


extern struct thread_info_list threads;
extern hts_mutex_t thread_info_mutex;
