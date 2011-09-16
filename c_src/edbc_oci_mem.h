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
#ifndef _EDBC_OCI_MEM_H
#define _EDBC_OCI_MEM_H

#include "edbc_oci.h"

#ifdef    __cplusplus
extern "C" {
#endif

#define DRV_ALLOC(p, size) safe_driver_alloc(p, size)

// NULL safe driver_free wrapper
#define DRV_FREE(x) if (x != NULL) driver_free(x)

void* safe_driver_alloc(void*, size_t);
void* zalloc(void*, size_t);
PropList* plist_alloc(void*, int*);

#ifdef _EDBC_OCI_HAVE_STDARG

#include <stdarg.h>

/* Tries to allocate heap space of 'size'. If this fails, the pointers
     in the varags array are freed one by one and the program fails. */
    void* try_driver_alloc(void*, size_t, void*, ...);

#endif

#ifdef    __cplusplus
}
#endif

#endif    /* _EDBC_OCI_MEM_H */

