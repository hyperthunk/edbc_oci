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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <erl_driver.h>
#include <ei.h>

#ifndef DEBUG
#define NDEBUG        // prevent assert from happening!
#endif

// we also allow test cases to override the *normal* assert behaviour
#ifndef _TEST_ASSERT
#include <assert.h>
#define ASSERT(stmt) assert(stmt)
#endif

#ifdef    __cplusplus
extern "C" {
#endif

/* logging */

#ifdef DEBUG
// TODO: define proper logging infrastructure
/* dev debugging hacks - not for use in test/prod */
// wrappers for fprintf (uses __VA_ARGS__ *unsafely* - dev builds only)
#define CONSOLE(str, ...) SPIT(stderr, str, __VA_ARGS__)
#define SPIT(stream, str, ...) fprintf(stream, str, ##__VA_ARGS__)
#else
#define CONSOLE(str, ...)
#endif

/* general utils */

// Cause the driver to fail (e.g., exit/unload)
#define FAIL(p, msg) driver_failure_atom(p, msg)

#ifdef    __cplusplus
}
#endif

#endif    /* _EDBC_OCI_H */
