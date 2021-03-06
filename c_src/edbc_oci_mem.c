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

#include "edbc_oci_mem.h"

void* zalloc(void *port, size_t size) {
    void* res = safe_driver_alloc(port, size);
    if(res != NULL) {
        memset(res, '\0', size);
    }
    return res;
};

void* safe_driver_alloc(void *port, size_t size) {
    void *p = driver_alloc(size);
    if (p == NULL) {
        FAIL(port, "system_limit");
    }
    return p;
};

PropList* plist_alloc(void *port, int *szalloc) {
    int allocated = *szalloc;
    PropList *plist = NULL;
    if ((plist = zalloc(port, sizeof(PropList))) != NULL) {
        *szalloc = ++allocated;
    }
    return plist;
};

void plist_free(PropList *plist) {
    PropList *next;
    while (plist != NULL) {
        next = (PropList*)plist->next;
        if (plist->name != NULL) {
            DRV_FREE(plist->name);
        }
        if (plist->type == EDBC_OCI_DRV_TYPE_STRING) {
            DRV_FREE(plist->value.buffer);
        }
        DRV_FREE(plist);
        plist = next;
    }
}

#ifdef _EDBC_OCI_HAVE_STDARG

// TODO: and if not!?

/* Tries to allocate heap space of 'size'. If this fails, the pointers
     in the varags array are freed one by one and the program fails. */
void* try_driver_alloc(void *port, size_t size, void* pfree, ...) {
    void* val = safe_driver_alloc(port, size);
    if (val == NULL) {
        va_list ap;
        void* p;

        va_start(ap, pfree);
        while ((p = va_arg(ap, void*)) != NULL) {
            DRV_FREE(p);
        }
        va_end(ap);

        FAIL(port, "system_limit");
    }
    return val;
};

#endif
