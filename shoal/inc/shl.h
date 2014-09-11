/*
 * Copyright (c) 2014 ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Universitaetsstrasse 6, CH-8092 Zurich. Attn: Systems Group.
 */

#ifndef __SHL_H
#define __SHL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include <stdint.h>
#include <sys/time.h>

#ifdef BARRELFISH
#include "barrelfish.h"
#endif

#ifdef LINUX
#include "linux.h"
#endif

#define noprintf(x,...) void(0)

#define BASE_UNIT 1024
#define KILO BASE_UNIT
#define MEGA (KILO*BASE_UNIT)
#define GIGA (MEGA*BASE_UNIT)
#define MAXCORES 100

// Size of a cacheline (bytes)
#define CACHELINE 8

// --------------------------------------------------
// Hardcoded page sizes
// --------------------------------------------------
#define PAGESIZE_HUGE (2*1024*1024)
#define PAGESIZE (4*1024)

#ifdef BARRELFISH
#define VERSION "1.0"
#endif

// --------------------------------------------------
// Configuration
// --------------------------------------------------

// Whether or not to use replication (needs: indirection OR copy)
#define REPLICATION

// Use NUMA aware memory allocation for replication
#define NUMA

// This does not seem to be currently used
//#define LOOKUP

// Add some additional debug checks! This will harm performance a LOT.
//#define DEBUG

// Enable PAPI support
//#define PAPI

// Enable support for hugepages
//#define ENABLE_HUGEPAGE

// --------------------------------------------------
// Typedefs
// --------------------------------------------------
#ifdef BARRELFISH
typedef uint8_t coreid_t; /// XXX: barrelfish has uint8_t as core id
#else
typedef uint32_t coreid_t;
#endif

// --------------------------------------------------
// in misc.c
// --------------------------------------------------
double shl__end_timer(void);
double shl__get_timer(void);
void shl__start_timer(void);
coreid_t *parse_affinity(bool);
char* get_env_str(const char*, const char*);
int get_env_int(const char*, int);
void print_number(long long);

// --------------------------------------------------
// OS specific stuff
// --------------------------------------------------
void shl__bind_processor(int proc);
void shl__bind_processor_aff(int proc);
int shl__get_proc_for_node(int node);
int shl__max_node(void);
long shl__node_size(int node, long *freep);
int numa_cpu_to_node(int);
void* shl__malloc(size_t size, int opts, int *pagesize, void **ret_mi);
void** shl_malloc_replicated(size_t, int*, int);
void loc(size_t, int, int*, void **);

// --------------------------------------------------
// SHOAL
// --------------------------------------------------
void shl__end(void);
void papi_stop(void);
void papi_init(void);
void papi_start(void);
int  shl__get_rep_id(void);
int  shl__lookup_rep_id(int);
void shl__repl_sync(void*, void**, size_t, size_t);
void shl__init_thread(int);
void handle_error(int);
int  shl__get_num_replicas(void);
void shl__init(size_t,bool);
int  shl__num_threads(void);
int  shl__get_tid(void);
int  shl__rep_coordinator(int);
bool shl__is_rep_coordinator(int);
// array helpers
// --------------------------------------------------
void** shl__copy_array(void *src, size_t size, bool is_used,
                       bool is_ro, const char* array_name);
void shl__copy_back_array(void **src, void *dest, size_t size, bool is_copied,
                          bool is_ro, bool is_dynamic, const char* array_name);
void shl__copy_back_array_single(void *src, void *dest, size_t size, bool is_copied,
                                 bool is_ro, bool is_dynamic, const char* array_name);


// --------------------------------------------------
// Auto-tuning interface
//
// Non of these functions are implemented. These are just meant as
// ideas for a future potential auto-tuning interface.
// --------------------------------------------------

/**
 * \brief Find sensible thread placement.
 *
 * Right now, a simple heuristic to decide how many threads to use is:
 * - 1) barriers -> one thread per physical core
 * - 2) no barriers -> one thread per HW context
 *
 * The thread placement would then be 1) to bind the threads such that
 * one thread is running on every physical core (i.e. not to have
 * threads on two HW contexts mapped to the same physical core) and 2)
 * one thread per HW context.
 *
 * But obviously, this is more complicated. Ultimately, we probably
 * want to specify an upper limit for the number of threads (maybe
 * also cores) to use (default: unlimited). Maybe also a bitmask of
 * CPUs to run on.
 *
 * The system then needs to figure out how many threads to spawn, and
 * where to place them. E.g. we know from newRTS that it is _not_
 * always good to spread out to several nodes. Somethings it is better
 * to use up all H/W threads on a node before going to the next one,
 * even if some of the threads will be hyperthreads.
 */
void shl__auto_tune_bind(int *num_cores,
                         coreid_t *bind,
                         bool uses_barriers);

extern int replica_lookup[];



// --------------------------------------------------
// Colors!
// --------------------------------------------------
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// --------------------------------------------------
// Defines for memory allocation
// --------------------------------------------------
#define SHL_MALLOC_NONE        (0)
#define SHL_MALLOC_HUGEPAGE    (0x1<<0)
#define SHL_MALLOC_DISTRIBUTED (0x1<<1)
#define SHL_MALLOC_PARTITION   (0x1<<2)

// --------------------------------------------------
// Includes depending on configuration
// --------------------------------------------------
#include <sys/mman.h>
#include <stdint.h>

#ifdef DEBUG
static uint64_t num_lookup = 0;
#endif

#ifdef __cplusplus
}
#endif

struct array_cache {

    int rid;
    int tid;
};

#endif
