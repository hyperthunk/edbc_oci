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

#ifndef _EDBC_OCI_DRV_H
#define _EDBC_OCI_DRV_H

#include "edbc_oci.h"
#include "edbc_oci_log.h"
#include "edbc_oci_mem.h"

#ifdef    __cplusplus
extern "C" {
#endif

#define EDBC_OCI_DRV_ERROR_GENERAL  -1

/* MAGICS */

// magics for command identification
// the corresponding erlang definitions can be found in edbc_oci_protocol.erl
#define INIT_OCI_COMMAND (uint32_t)1
#define OTHER_COMMAND (uint32_t)2

typedef enum {
    READER_NO_ERROR         = 0,
    READER_READ_ALL_DATA    = 1,
    READER_PACKING_ERROR    = 2
} ReaderError;

typedef struct {
    ErlIOVec *    ev;
    size_t        row;
    size_t        column;
    ReaderError   last_error;
} Reader;

static bool decode_proplist(void*, PropList**, int*, const char*, int*);
static ErlDrvData start_driver(ErlDrvPort, char*);
static void stop_driver(ErlDrvData);
/* static int call(ErlDrvData, unsigned int, char*, int, char**, int, unsigned int*); */
static void outputv(ErlDrvData, ErlIOVec*);

#ifdef    __cplusplus
}
#endif

#endif    /* _EDBC_OCI_DRV_H */
