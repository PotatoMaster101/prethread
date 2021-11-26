///////////////////////////////////////////////////////////////////////////////
// Simple pre-threading in C99. Uses pthread library.
// https://github.com/PotatoMaster101/prethread
///////////////////////////////////////////////////////////////////////////////

#ifndef PRETHD_H
#define PRETHD_H
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// Represents pre-allocated threads.
typedef struct pre_threads_t prethd_t;

// Allocate a new pool of threads.
//
// PARAMS:
// th   - number of threads in the pool
// mut  - number of mutexes in the pool
// cond - number of conditional variables in the pool
//
// RETURN:
// Allocated pool of threads, or NULL on error.
prethd_t *prethd_new(size_t th, size_t mut, size_t cond);

// Starts all the threads in the given thread pool.
//
// PARAMS:
// th   - the thread pool to start
// func - function for all threads to run
// arg  - argument for the function
//
// RETURN:
// The number of threads started, 0 on error.
size_t prethd_all(prethd_t *th, void *(*func)(void *), void *arg);

// Returns the number of threads in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool size, or 0 on error.
size_t prethd_size(prethd_t *th);

// Returns the number of mutexes in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool mutex size, or 0 on error.
size_t prethd_mutex_size(prethd_t *th);

// Returns the number of conditional variables in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool conditional variable size, or 0 on error.
size_t prethd_cond_size(prethd_t *th);

// Joins all threads in the thread pool.
//
// PARAMS:
// th - the thread pool to join
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_join(prethd_t *th);

// Locks the given thread pool.
//
// PARAMS:
// th - the thread pool to lock
// i  - the index of mutex to lock
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_lock(prethd_t *th, size_t i);

// Unocks the given thread pool.
//
// PARAMS:
// th - the thread pool to unlock
// i  - the index of mutex to unlock
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_unlock(prethd_t *th, size_t i);

// Make the given thread pool wait on the conditional variable.
//
// PARAMS:
// th - the thread pool to wait
// c  - the conditional variable index to wait
// m  - the mutex index to use
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_wait(prethd_t *th, size_t c, size_t m);

// Signals the conditional variable in the given thread pool.
//
// PARAMS:
// th - the thread pool to signal
// i  - the conditional variable index to signal
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_signal(prethd_t *th, size_t i);

// Broadcasts the conditional variable in the given thread pool, waking up
// all.
//
// PARAMS:
// th - the thread pool to broadcast
// i  - the conditional variable index to broadcast
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_broad(prethd_t *th, size_t i);

// Frees the specified thread pool.
//
// PARAMS:
// th - the thread pool to free
void prethd_free(prethd_t *th);

// Frees the specified thread pool. Waits for all threads to join before
// freeing.
//
// PARAMS:
// th - the thread pool to free
void prethd_join_free(prethd_t *th);

#endif
