/*
 * edbc_oci_drv.c
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2011 Tim Watson (watson.timothy@gmail.com)
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

#include <erl_driver.h>

ErlDrvTermData*
make_driver_term(ErlDrvPort*, ErlDrvTermData*, char*, long*);
int send_tagged_data(ErlDrvPort*, ErlDrvTermData,
                     ErlDrvTermData*, char*);

ErlDrvTermData*
make_driver_term(ErlDrvPort *port, ErlDrvTermData *tag,
                 char *payload, long *length) {
    ErlDrvTermData *term;
    ErlDrvTermData    spec[9];
    term = driver_alloc(sizeof(spec));
    if (term == NULL) return NULL;

    spec[0] = ERL_DRV_ATOM;
    spec[1] = *tag;
    spec[2] = ERL_DRV_PORT;
    spec[3] = driver_mk_port(*port);
    spec[4] = ERL_DRV_BUF2BINARY;
    spec[5] = (ErlDrvTermData)payload;
    spec[6] = strlen(payload);
    spec[7] = ERL_DRV_TUPLE;
    spec[8] = 3;

    memcpy(term, &spec, sizeof(spec));
    *length = sizeof(spec) / sizeof(spec[0]);
    return term;
};

// TODO: figure out what other error codes are appropriate
/* Returns 0 on success, ENOMEM if out of memory, ... */
int send_tagged_data(ErlDrvPort *port, ErlDrvTermData callee,
                     ErlDrvTermData *tag, char *payload) {
    if (tag == payload == NULL) {
        return EINVAL;
    }

    long len;
    ErlDrvTermData *term = make_driver_term(port, tag, payload, &len);
    if (term == NULL) {
        return ENOMEM;
    }

    return driver_send_term(port, callee, term, len);
};
