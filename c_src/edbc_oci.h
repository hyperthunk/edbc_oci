/*
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-2010 Tim Watson (watson.timothy@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#ifndef _EDBC_OCI_H
#define _EDBC_OCI_H

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#ifdef _EDBC_OCI_HAVE_STDINT
#include <stdint.h>
#else
#error "stdint header is required"
#endif

#ifdef _EDBC_OCI_HAVE_STDARG
#include <stdarg.h>
#else
#error support for variadic functions is required to build this module!
#endif

#include <erl_driver.h>

// #ifndef DEBUG
// #define NDEBUG        // prevent assert from happening!
// #endif

// we also allow test cases to override the *normal* assert behaviour
#ifndef _TEST_ASSERT
#include <assert.h>
#define ASSERT(stmt) assert(stmt)
#endif

#ifdef    __cplusplus
extern "C" {
#endif

#define EDBC_OCI_DRV_TYPE_UNASSIGNED 0l
#define EDBC_OCI_DRV_TYPE_STRING     2l
#define EDBC_OCI_DRV_TYPE_LONG       3l

#define INITIAL_THREAD_POOL_COUNT 1
#define PORT_LABEL_SIZE 64

typedef struct logging_descriptor {
    FILE *              fd;     /* Used with atomic write() ops only */
    uint32_t            mask;   /* Used to control logging levels and options */
    ErlDrvMutex *       mutex;      /* for avoiding interleaving fprintf output */

    // Invariants
    //      if (mask & ENABLE_SASL_LOGGING) == true
    //      then sasl_logging_pid must point to a valid (running) process
    //      else send_term_mutex == NULL

    bool                send_term_requires_lock; /* if we are in SMP mode? */
    ErlDrvTermData      sasl_logging_pid;   /* pid for the sasl logging wrapper */
    ErlDrvMutex *       send_term_mutex; /* shared ~ EdbcOciPortData->mutex */
    ErlDrvPort          port;           /* used by send_term */
    ErlDrvTermData      log_msg_tag;    /* atom tag for logging messages */
} Logger;

/* much of this has been taken/adapted from edtk, which is turn in was adapted from
the code in erl_async.c - the major difference is that we don't require the calls
that return data to the client process to run in an emulator thread, so we don't
worry about signalling that jobs have completed. */

typedef struct pool_entry {
    /* these fields are only accessed by code holding
     * the pool_t->mutex and are not touched otherwise */
    struct pool_entry *         next;
    struct pool_entry *         prev;

    /* protects the client_* and async_* fields */
    ErlDrvMutex *               state_mutex;
    bool                        client_cancelled;
    ErlDrvTermData              client_pid;
    ErlDrvMonitor *             client_monitor;

    /* async data/functions assigned when a task is submitted */
    void *                      async_data;
    void                        (*async_invoke)(void*);
    void                        (*async_free)(void*);

    Logger *                    log;

    /* thread id used to join during termination routines */
    ErlDrvTid                   tid;
} PoolEntry;

typedef struct {
    char                    label[64];
    Logger *                log;

    ErlDrvMutex *           mutex;  /* general mutex; protects queue and all other modifyable members */
    ErlDrvCond *            queue_pending_cv;
    PoolEntry *             queue_head;
    PoolEntry *             queue_tail;
    unsigned int            queue_len;
    unsigned int            queue_limit;

    /* Invariants
     *  0 <= num_created_threads
     *  0 <= curr_enqueued_poison_pills
     *  0 <= (num_created_threads - curr_enqueued_poison_pills)
     *  0 <= curr_idle_threads <= (num_created_threads - curr_enqueued_poison_pills)
     *
     * We do allow set_thread_count to 0 (new operations on that threadpool will be queued up
     * until threads are added -- should rarely if ever be needed of course).
     */
    int                   num_created_threads;
    int                   curr_enqueued_poison_pills;
    int                   curr_idle_threads;

} WorkerPool;

/* Port Data - we need this defined up front as it's used elsewhere... */
typedef struct {
    ErlDrvPort              port;           /* The port instance */
    ErlDrvMutex *           mutex;          /* Protects this data structure */
    ErlDrvTermData          owner;          /* Pid of the owning process */
    Logger *                log;            /* Logger structure */
    WorkerPool *            worker_pool;    /* private thread pool */
    /*
    WorkerPool *            house_keeper;  */
    char                    label[PORT_LABEL_SIZE + 1];
    bool                    send_term_requires_lock; /* if we are in SMP mode? */
} EdbcOciPortData;

typedef struct {
    int32_t size; // do we know for sure this is big enough?
    char *data;
} TextBuffer;

typedef struct plist {
    char* name;
    long type;
    union {
        TextBuffer *buffer;
        long number;
    } value;
    void *next;
} PropList;

/* general utils */

// Cause the driver to fail (e.g., exit/unload)
#ifndef FAIL
#define FAIL(p, msg) driver_failure_atom(p, msg)
#endif

#ifdef    __cplusplus
}
#endif

#endif    /* _EDBC_OCI_H */
