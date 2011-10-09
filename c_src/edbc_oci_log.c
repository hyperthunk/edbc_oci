/*
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
 *
 */

#include "edbc_oci_log.h"

Logger* open_log(EdbcOciPortData *pd, const char *log_file, uint32_t modes) {
    Logger *plogger = DRV_ALLOC(pd->port, sizeof(Logger));
    if (plogger) {
        plogger->mask = modes;
        if (plogger->mutex = erl_drv_mutex_create("edbc_oci_log_mutex")) {
            if ((plogger->fd = fopen(log_file, "a")) == NULL) {
                FAIL(pd->port, "edbc_oci_log_file_open");
            }
        } else {
            FAIL(pd->port, "edbc_oci_log_mutex_create");
        }
        
        // we suck up a little duplication for the simplicity it buys us
        plogger->port = pd->port;
        plogger->send_term_requires_lock = pd->send_term_requires_lock;
        if (plogger->send_term_requires_lock) {
            plogger->send_term_mutex = pd->mutex;
        }
        
        plogger->log_msg_tag = driver_mk_atom("port_logging");
    }
    return plogger;
};

// we don't bother with setting individual options, as there are few of them
// to consider and the erlang code might just as well compute the lot in 1 go
void set_flags(Logger *logger, uint32_t flags) {
    if (logger != NULL) {
        logger->mask = flags;
    }
};

// this function is very busy and probably a total drag on performance, given
// that it is usually disabled for most logging statements, we'll let it stay
int write_log(uint32_t level, Logger *logger, char* format, ...) {
    // level is open to nonsensical use, but we dont publish this api anyway
    int written;
    if (logger->mask & level) {
        va_list ap;
        void* p;
        va_start(ap, format);
        erl_drv_mutex_lock(logger->mutex);
        written = vfprintf(logger->fd, ((const char *)format), ap);
        erl_drv_mutex_unlock(logger->mutex);

        if (written >= 0 && (logger->mask & ENABLE_SASL_LOGGING)) {
            // return write_sasl_log(logger, ((size_t)written + 1), format, ap);
            size_t size = ((size_t)written + 1);
            char *payload = driver_alloc(size);
            if (payload != NULL && (snprintf(payload, size, format, ap) > 0)) {
                if (logger->send_term_requires_lock) {
                    // a more coarse grained locking is required
                    erl_drv_mutex_lock(logger->send_term_mutex);
                    return send_tagged_data(logger->port,
                                            logger->sasl_logging_pid,
                                            &(logger->log_msg_tag), payload);
                    erl_drv_mutex_unlock(logger->send_term_mutex);
                } else {
                    return send_tagged_data(logger->port,
                                            logger->sasl_logging_pid,
                                            &(logger->log_msg_tag), payload);
                }
            }
            DRV_FREE(payload);
        }
        va_end(ap);
    }
    return result;
};
