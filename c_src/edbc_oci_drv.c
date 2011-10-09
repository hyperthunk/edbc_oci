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
 * Notes:
 *
 */

#include "edbc_oci_drv.h"

/* INTERNAL DATA & DATA STRUCTURES */

#define NUM_TYPE_HEADERS 3
#define NUM_SIZE_HEADERS 2
#define FIRST_BINV_ENTRY 1

/* PROTOCOL HANDLING FUNCTIONS */

static bool
decode_proplist(void *port, PropList **plist, int *szalloc,
                const char *buf, int *index) {
    int type, size;
    long item_type;
    bool succeess = false;
    PropList *list_head = (*plist = plist_alloc(port, szalloc));
    if (DECODED(ei_get_type(buf, index, &type, &size))) {
        int list_arity = 0;
        if (DECODED(ei_decode_list_header(buf, index, &list_arity))) {
            ASSERT(size == list_arity);
            if (list_arity < 1) {
                succeess = false;
            } else {
                for (int i = 0; i < list_arity; i++) {
                    if (!DECODED(ei_decode_tuple_header(buf, index, &size))) {
                        return false;
                    }
                    if ((list_head->next = plist_alloc(port, szalloc)) != NULL) {
                        *plist = (list_head = list_head->next);
                        list_head->type = EDBC_OCI_DRV_TYPE_UNASSIGNED;
                        char *pkey = safe_driver_alloc(port, sizeof(char) * MAXATOMLEN);
                        
                        if (!DECODED(ei_decode_atom(buf, index, pkey))) {
                            return false;
                        } 
                        
                        list_head->name = pkey;
                        
                        if (!DECODED(ei_decode_long(buf, index, &item_type))) {
                            return false;
                        }
                        list_head->type = item_type;
                        
                        switch (item_type) {
                        case EDBC_OCI_DRV_TYPE_STRING: {
                            if(!DECODED(ei_get_type(buf, index, &type, &size))) {
                                return false;
                            }
                            ASSERT(type == ERL_STRING_EXT);
                            TextBuffer *ptxt = zalloc(port, sizeof(TextBuffer));
                            char *pval = safe_driver_alloc(port, sizeof(char) * ptxt->size);

                            ptxt->size = size + 1;
                            ptxt->data = pval;
                            list_head->value.buffer = ptxt;
                            if (!DECODED(ei_decode_string(buf, index, pval))) {
                                return false;
                            }
                            break;
                        }
                        case EDBC_OCI_DRV_TYPE_LONG: {
                            long num;
                            if (!DECODED(ei_decode_long(buf, index, &num))) {
                                return false;
                            }
                            list_head->value.number = num;
                            break;
                        }
                        default:
                            // make error?
                            break;
                        }
                    }
                }
            }
        }
    }
    return false;
};

/* DRIVER CALLBACK FUNCTIONS */

// Called by the emulator when the driver is starting.
static ErlDrvData
start_driver(ErlDrvPort port, char *buff) {
    EdbcOciPortData *pd = zalloc(port, sizeof(EdbcOciPortData));
    if (pd == NULL) {
        // TODO: use ERL_DRV_ERROR_ERRNO and provide out-of-memory info
        return ERL_DRV_ERROR_GENERAL;
    }

    ErlDrvSysInfo sys_info;
    driver_system_info(&sys_info, sizeof(ErlDrvSysInfo));
    pd->send_term_requires_lock = (sys_info->smp_support == 0);
    pd->port = (void*)port;
    pd->owner = driver_connected(port);
    pd->mutex = erl_drv_mutex_create("edbc_oci_port_instance_mutex");

    // some fields can't be initialized until we've got our hands on 
    // the relevant configuration data at runtime...
    pd->worker_pool = NULL;
    pd->log = NULL;
    
    return (ErlDrvData)pd;
};

// Called by the emulator when the driver is stopping.
static void
stop_driver(ErlDrvData drv_data) {

    driver_free(drv_data);
};

/*
This function is called whenever the port is written to. The port should be in binary mode, see open_port/2.
The ErlIOVec contains both a SysIOVec, suitable for writev, and one or more binaries. If these binaries should be retained,
when the driver returns from outputv, they can be queued (using driver_enq_bin for instance), or if they are kept in a
static or global variable, the reference counter can be incremented.
*/
static void
outputv(ErlDrvData drv_data, ErlIOVec *ev) {

};

/* DRIVER API EXPORTS */

static ErlDrvEntry driver_entry = {
    NULL,               /* init */
    start_driver,       /* start, called when port is opened */
    stop_driver,        /* stop, called when port is closed */
    NULL,               /* output, called when port receives messages */
    NULL,               /* ready_input, called when input descriptor ready to read*/
    NULL,               /* ready_output, called when output descriptor ready to write */
    "liboci_drv",       /* the name of the driver */
    NULL,               /* finish, called when unloaded */ //TODO: should we do resource disposal here!?
    NULL,               /* handle,    */
    NULL,               /* control */
    NULL,               /* timeout */
    outputv,            /* outputv */
    NULL,               /* ready_async, called (from the emulator thread) after an asynchronous call has completed. */
    NULL,               /* flush */
    call,               /* call */
    NULL                /* event */
};

DRIVER_INIT(liboci_drv) {
    return &driver_entry;
}
