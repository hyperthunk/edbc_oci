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

#ifndef _EDBC_OCI_LOG_H
#define _EDBC_OCI_LOG_H

#include "edbc_oci.h"

#ifdef    __cplusplus
extern "C" {
#endif

#define ENABLE_DEBUG_LOGGING        ((uint32_t)0x00000001)
#define ENABLE_INFO_LOGGING         ((uint32_t)0x00000002)
#define ENABLE_WARN_LOGGING         ((uint32_t)0x00000004)
#define ENABLE_ERROR_LOGGING        ((uint32_t)0x00000008)
#define RESERVED_MASK_LOGGING       ((uint32_t)0x40000000)
#define ENABLE_SASL_LOGGING         ((uint32_t)0x80000000)

#define DEBUG(L, M, ...)            LOG(ENABLE_DEBUG_LOGGING, L, M, ##__VA_ARGS__)
#define INFO(L, M, ...)             LOG(ENABLE_INFO_LOGGING, L, M, ##__VA_ARGS__)
#define WARN(L, M, ...)             LOG(ENABLE_WARN_LOGGING, L, M, ##__VA_ARGS__)
#define ERROR(L, M, ...)            LOG(ENABLE_ERROR_LOGGING, L, M, ##__VA_ARGS__)
#define LOG(lv, lg, fmt, ap)        write_log(lv, lg, fmt, ap)

Logger*
open_log(EdbcOciPortData*   /*port_data*/,
         const char*        /*log_file*/,
         uint32_t           /*modes*/);

void
set_flags(Logger*           /*logger*/,
          uint32_t          /*flags*/);

int
write_log(uint32_t          /*level*/,
          Logger*           /*logger*/,
          char*             /*format*/,
          ...);

#ifdef    __cplusplus
}
#endif

#endif    /* _EDBC_OCI_LOG_H */
