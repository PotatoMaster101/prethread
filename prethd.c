///////////////////////////////////////////////////////////////////////////////
// Simple pre-threading in C99. Uses pthread library.
// https://github.com/PotatoMaster101/prethread
///////////////////////////////////////////////////////////////////////////////

#include "prethd.h"

// Pre-allocated threads.
struct pre_threads_t {
    pthread_mutex_t *muts;      // list of mutexes for locking
    pthread_cond_t *conds;      // list of conditional variables
    pthread_t *threads;         // threads in the pool
    size_t len;                 // number of threads
    size_t mlen;                // number of mutexes
    size_t clen;                // number of conditional variables
};

static pthread_mutex_t *init_muts(size_t n);
static void des_muts(pthread_mutex_t *muts, size_t n);
static pthread_cond_t *init_conds(size_t n);
static void des_conds(pthread_cond_t *conds, size_t n);

// Allocate a new pool of threads.
//
// PARAMS:
// th   - number of threads in the pool
// mut  - number of mutexes in the pool
// cond - number of conditional variables in the pool
//
// RETURN:
// Allocated pool of threads, or NULL on error.
prethd_t *prethd_new(size_t th, size_t mut, size_t cond) {
    if (th == 0)
        return NULL;

    prethd_t *ret = malloc(sizeof *ret);
    if (ret != NULL) {
        ret->len = th;
        ret->mlen = mut;
        ret->clen = cond;
        ret->muts = init_muts(mut);
        ret->conds = init_conds(cond);
        ret->threads = malloc(th * sizeof(pthread_t));
        if (ret->threads == NULL) {
            des_muts(ret->muts, mut);
            des_conds(ret->conds, cond);
            free(ret);
            ret = NULL;
        }
    }
    return ret;
}

// Starts all the threads in the given thread pool.
//
// PARAMS:
// th   - the thread pool to start
// func - function for all threads to run
// arg  - argument for the function
//
// RETURN:
// The number of threads started, 0 on error.
size_t prethd_all(prethd_t *th, void *(*func)(void *), void *arg) {
    if (th == NULL)
        return 0;

    size_t ret = 0;
    for (size_t i = 0; i < th->len; i++) {
        if (pthread_create(&(th->threads[i]), NULL, func, arg) != 0)
            break;      // pthread_create() error
        ret++;
    }
    return ret;
}

// Returns the number of threads in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool size, or 0 on error.
size_t prethd_size(prethd_t *th) {
    return (th == NULL) ? 0 : th->len;
}

// Returns the number of mutexes in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool mutex size, or 0 on error.
size_t prethd_mutex_size(prethd_t *th) {
    return (th == NULL) ? 0 : th->mlen;
}

// Returns the number of conditional variables in the thread pool.
//
// PARAMS:
// th - the thread pool to retrieve the size
//
// RETURN:
// The thread pool conditional variable size, or 0 on error.
size_t prethd_cond_size(prethd_t *th) {
    return (th == NULL) ? 0 : th->clen;
}

// Joins all threads in the thread pool.
//
// PARAMS:
// th - the thread pool to join
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_join(prethd_t *th) {
    if (th == NULL)
        return false;

    int chk = 0;
    for (size_t i = 0; i < th->len; i++)
        chk += pthread_join(th->threads[i], NULL);
    return chk == 0;
}

// Locks the given thread pool.
//
// PARAMS:
// th - the thread pool to lock
// i  - the index of mutex to lock
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_lock(prethd_t *th, size_t i) {
    return (th == NULL || i >= th->mlen) ? false :
        pthread_mutex_lock(th->muts + i) == 0;
}

// Unocks the given thread pool.
//
// PARAMS:
// th - the thread pool to unlock
// i  - the index of mutex to unlock
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_unlock(prethd_t *th, size_t i) {
    return (th == NULL || i >= th->mlen) ? false :
        pthread_mutex_unlock(th->muts + i) == 0;
}

// Make the given thread pool wait on the conditional variable.
//
// PARAMS:
// th - the thread pool to wait
// c  - the conditional variable index to wait
// m  - the mutex index to use
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_wait(prethd_t *th, size_t c, size_t m) {
    return (th == NULL || c >= th->clen || m >= th->mlen) ? false :
        pthread_cond_wait(th->conds + c, th->muts + m) == 0;
}

// Signals the conditional variable in the given thread pool.
//
// PARAMS:
// th - the thread pool to signal
// i  - the conditional variable index to signal
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_signal(prethd_t *th, size_t i) {
    return (th == NULL || i >= th->clen) ? false :
        pthread_cond_signal(th->conds + i) == 0;
}

// Broadcasts the conditional variable in the given thread pool, waking up
// all.
//
// PARAMS:
// th - the thread pool to broadcast
// i  - the conditional variable index to broadcast
//
// RETURN:
// 1 (true) on success, 0 (false) on error.
_Bool prethd_broad(prethd_t *th, size_t i) {
    return (th == NULL || i >= th->clen) ? false :
        pthread_cond_broadcast(th->conds + i) == 0;
}

// Frees the specified thread pool.
//
// PARAMS:
// th - the thread pool to free
void prethd_free(prethd_t *th) {
    if (th != NULL) {
        des_muts(th->muts, th->mlen);
        des_conds(th->conds, th->clen);
        free(th->threads);
        free(th);
    }
}

// Frees the specified thread pool. Waits for all threads to join before
// freeing.
//
// PARAMS:
// th - the thread pool to free
void prethd_join_free(prethd_t *th) {
    if (prethd_join(th)) {
        des_muts(th->muts, th->mlen);
        des_conds(th->conds, th->clen);
        free(th->threads);
        free(th);
    }
}

// Returns an array of mutexes.
//
// PARAMS:
// n - the number of mutexes to create
//
// RETURN:
// The array of mutexes, or NULL on error.
static pthread_mutex_t *init_muts(size_t n) {
    if (n == 0)
        return NULL;

    pthread_mutex_t *mut = malloc(n * (sizeof *mut));
    if (mut != NULL)
        for (size_t i = 0; i < n; i++)
            pthread_mutex_init(mut + i, NULL);
    return mut;
}

// Destroyes the array of mutexes.
//
// PARAMS:
// muts - the array of mutexes to destroy
// n    - the count of mutexes
static void des_muts(pthread_mutex_t *muts, size_t n) {
    if (muts != NULL && n > 0) {
        for (size_t i = 0; i < n; i++)
            pthread_mutex_destroy(muts + i);
        free(muts);
    }
}

// Returns an array of conditional variables.
//
// PARAMS:
// n - the number of conditional variables to create
//
// RETURN:
// The array of conditional variables, or NULL on error.
static pthread_cond_t *init_conds(size_t n) {
    if (n == 0)
        return NULL;

    pthread_cond_t *cond = malloc(n * (sizeof *cond));
    if (cond != NULL)
        for (size_t i = 0; i < n; i++)
            pthread_cond_init(cond + i, NULL);
    return cond;
}

// Destroyes the array of conditional variables.
//
// PARAMS:
// conds - the conditional variables to destroy
// n     - the count of conditional variables
static void des_conds(pthread_cond_t *conds, size_t n) {
    if (conds != NULL && n > 0) {
        for (size_t i = 0; i < n; i++)
            pthread_cond_destroy(conds + i);
        free(conds);
    }
}
