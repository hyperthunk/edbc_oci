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
    PropList *list_head = (*plist = plist_alloc(port, szalloc));
    if (DECODED(ei_get_type(buf, index, &type, &size))) {
        int list_arity = 0;
        if (DECODED(ei_decode_list_header(buf, index, &list_arity))) {
            ASSERT(size == list_arity);
            if(list_arity < 1) return false;

            
            for (int i = 0; i < list_arity; i++) {
                if (!DECODED(ei_decode_tuple_header(buf, index, &size))) {
                    return false;
                }
                if ((list_head->next = plist_alloc(port, szalloc)) != NULL) {
                    *plist = (list_head = list_head->next);
                    char *pkey = safe_driver_alloc(port, sizeof(char) * MAXATOMLEN);
                    if (DECODED(ei_decode_atom(buf, index, pkey))) {
                        list_head->name = pkey;
                    }
                    if (!DECODED(ei_decode_long(buf, index, &item_type))) {
                        return false;
                    }
                    switch (item_type) {
                    case EDBC_OCI_DRV_TYPE_STRING:
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
                    case EDBC_OCI_DRV_TYPE_LONG:
                        // ei_decode_long(buf, index, &number)
                    default:
                        // make error?
                        break;
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
    DriverData *d = zalloc(port, sizeof(DriverData));
    if (d == NULL) {
        // TODO: use ERL_DRV_ERROR_ERRNO and provide out-of-memory info
        return ERL_DRV_ERROR_GENERAL;
    }
    d->port = (void*)port;
    d->port_owner = driver_connected(port);
    d->port_lock = erl_drv_mutex_create("bdberl_port_lock");
    return (ErlDrvData)d;
};

// Called by the emulator when the driver is stopping.
static void
stop_driver(ErlDrvData drv_data) {

    driver_free(drv_data);
};

/*
This function is called from erlang:port_call/3. It works a lot like the control call-back,
but uses the external term format for input and output.
- command is an integer, obtained from the call from erlang (the second argument to erlang:port_call/3).
- buf and len provide the arguments to the call (the third argument to erlang:port_call/3). They're decoded using ei.
- rbuf points to a return buffer, rlen bytes long.

The return data (written to *rbuf) should be a valid erlang term in the external term format. This is converted to an
erlang term and returned by erlang:port_call/3 to the caller. If more space than rlen bytes is needed to return data,
*rbuf can be set to memory allocated with driver_alloc. This memory will be freed automatically after call has returned.
The return value (of this callback function) is the number of bytes returned in *rbuf. If ERL_DRV_ERROR_GENERAL is returned
(or in fact, anything < 0), erlang:port_call/3 will throw a BAD_ARG.
*/
static int
call(ErlDrvData drv_data, unsigned int command, char *buf,
    int len, char **rbuf, int rlen, unsigned int *flags) {

    int i;
    int index = 0;
    int rindex = 0;
    char *data;
    DriverData *d = (DriverData*)drv_data;

    if(!DECODED(ei_decode_version(buf, &index, &i))) {
        return EDBC_OCI_DRV_ERROR_GENERAL;
    };

    if (command == INIT_OCI_COMMAND) {
        PropList *plist;
        int szalloc = 0;
        if (decode_proplist(d->port, &plist, &szalloc, buf, &index)) {
            // do something with it....
        }
    }

    ei_encode_version(*rbuf, &rindex);
#ifdef _DRV_SASL_LOGGING
    // TODO: pull the logging_port and install it....
#endif
    ei_encode_atom(*rbuf, &rindex, "ok");
    DRV_FREE(data);
    return(rindex);
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

/*
This function is called after an asynchronous call has completed. The asynchronous
call is started with driver_async. This function is called from the erlang emulator thread,
as opposed to the asynchronous function, which is called in some thread (if multithreading is enabled).
*/
static void
ready_async(ErlDrvData drv_data, ErlDrvThreadData data) {

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
    ready_async,        /* ready_async, called (from the emulator thread) after an asynchronous call has completed. */
    NULL,               /* flush */
    call,               /* call */
    NULL                /* event */
};

DRIVER_INIT(liboci_drv) {
    return &driver_entry;
}
