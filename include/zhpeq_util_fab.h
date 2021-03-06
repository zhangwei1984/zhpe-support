/*
 * Copyright (C) 2017-2018 Hewlett Packard Enterprise Development LP.
 * All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ZHPEQ_UTIL_FAB_H_
#define _ZHPEQ_UTIL_FAB_H_

#include <zhpeq_util.h>

#include <rdma/fabric.h>
#include <rdma/fi_atomic.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_rma.h>
#include <rdma/fi_tagged.h>
#include <rdma/fi_trigger.h>

#define FAB_FIVERSION       FI_VERSION(1, 4)

#define FI_CLOSE(_ptr)                                          \
do {                                                            \
    typeof (_ptr)__ptr = (_ptr);                                \
    if (__ptr) {                                                \
        fi_close(&_ptr->fid);                                   \
        _ptr = NULL;                                            \
    }                                                           \
} while (0)

struct fab_mrmem {
    struct fid_mr       *mr;
    void                *mem;
};

struct fab_conn {
    char                *node;
    char                *service;
    struct fi_info      *hints;
    struct fi_info      *info;
    struct fid_fabric   *fabric;
    struct fid_domain   *domain;
    struct fid_eq       *eq;
    struct fid_cq       *tx_cq;
    struct fid_cq       *rx_cq;
    struct fid_av       *av;
    struct fid_ep       *ep;
    struct fid_pep      *pep;
    struct fab_mrmem    mrmem;
    bool                allocated;
};

/* Wasteful, but saves code changes. */
struct fab_dom {
    struct fab_conn     fab_conn;
};

static inline void print_func_fi_err(const char *callf, uint line,
                                     const char *errf, const char *arg,
                                     int err)
{
    char                *estr = NULL;

    if (errf)
        estr = errf_str("%s(%s)", errf, arg);
    if (err < 0)
        err = -err;

    print_errs(callf, line, estr, err, fi_strerror(err));
}

static inline void print_func_fi_errn(const char *callf, uint line,
                                      const char *errf, llong arg,
                                      bool arg_hex, int err)
{
    char                *estr = NULL;

    if (errf)
        estr = errf_str((arg_hex ? "%s(0x%Lx)" : "%s(%Ld)"), errf, arg);
    if (err < 0)
        err = -err;

    print_errs(callf, line, estr, err, fi_strerror(err));
}

void fab_conn_init(struct fab_dom *dom, struct fab_conn *conn);

static inline void fab_dom_init(struct fab_dom *dom)
{
    fab_conn_init(NULL, &dom->fab_conn);
}

struct fab_conn *_fab_conn_alloc(const char *callf, uint line,
                                 struct fab_dom *dom);

#define fab_conn_alloc(...) \
    _fab_conn_alloc(__FUNCTION__, __LINE__, __VA_ARGS__)

int fab_conn_free(struct fab_conn *conn);

int _fab_getinfo(const char *callf, uint line,
                 const char *service, const char *node,
                 const char *provider, const char *domain,
                 enum fi_ep_type ep_type, bool passive, struct fab_conn *conn);

#define fab_getinfo(...) \
    _fab_getinfo(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_listener_setup(const char *callf, uint line, int backlog,
                        struct fab_conn *listener);

#define fab_listener_setup(...) \
    _fab_listener_setup(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_listener_wait_and_accept(const char *callf, uint line,
                                  struct fab_conn *listener, int timeout,
                                  size_t tx_size, size_t rx_size,
                                  struct fab_conn *conn);

#define fab_listener_wait_and_accept(...) \
    _fab_listener_wait_and_accept(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_connect(const char *callf, uint line, int timeout,
                 size_t tx_size, size_t rx_size, struct fab_conn *conn);

#define fab_connect(...) \
    _fab_connect(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_domain(const char *callf, uint line, const char *provider,
                  const char *domain, struct fab_dom *dom);

#define fab_av_domain(...) \
    _fab_av_domain(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_ep(const char *callf, uint line, struct fab_conn *conn,
               size_t tx_size, size_t rx_size);

#define fab_av_ep(...) \
    _fab_av_ep(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_xchg_addr(const char *callf, uint line, struct fab_conn *conn,
                 int sock_fd, union sockaddr_in46 *ep_addr);

#define fab_av_xchg_addr(...) \
    _fab_av_xchg_addr(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_xchg(const char *callf, uint line, struct fab_conn *conn,
                 int sock_fd, int timeout, fi_addr_t *fi_addr);

#define fab_av_xchg(...) \
    _fab_av_xchg(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_insert(const char *callf, uint line, struct fab_conn *conn,
                   union sockaddr_in46 *saddr, fi_addr_t *fi_addr);

#define fab_av_insert(...) \
    _fab_av_insert(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_remove(const char *callf, uint line, struct fab_conn *conn,
                   fi_addr_t fi_addr);

#define fab_av_remove(...) \
    _fab_av_remove(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_wait_send(const char *callf, uint line, struct fab_conn *conn,
                      fi_addr_t fi_addr,
                      int (*retry)(void *retry_arg), void *retry_arg);

#define fab_av_wait_send(...) \
    _fab_av_wait_send(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_av_wait_recv(const char *callf, uint line, struct fab_conn *conn,
                      fi_addr_t fi_addr,
                      int (*retry)(void *retry_arg), void *retry_arg);

#define fab_av_wait_recv(...) \
    _fab_av_wait_recv(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_listener_setup(const char *callf, uint line, int backlog,
                        struct fab_conn *listener);
#define fab_listener_setup(...) \
    _fab_listener_setup(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_ep_setup(const char *callf, uint line,
                  struct fab_conn *conn, struct fid_eq *eq,
                  size_t tx_size, size_t rx_size);

#define fab_ep_setup(...) \
    _fab_ep_setup(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_eq_cm_event(const char *callf, uint line,
                     struct fab_conn *conn, int timeout, uint32_t expected,
                     struct fi_eq_cm_entry *entry);

#define fab_eq_cm_event(...) \
    _fab_eq_cm_event(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_mrmem_alloc(const char *callf, uint line, struct fab_conn *conn,
                     struct fab_mrmem *mrmem, size_t len, uint64_t access);

#define fab_mrmem_alloc(...) \
    _fab_mrmem_alloc(__FUNCTION__, __LINE__, __VA_ARGS__)

void fab_mrmem_free(struct fab_mrmem *mrmem);

ssize_t _fab_completions(const char *callf, uint line,
                         struct fid_cq *cq, size_t count,
                         void (*cq_update)(void *arg, void *cqe, bool err),
                         void *arg);

#define fab_completions(...) \
    _fab_completions(__FUNCTION__, __LINE__, __VA_ARGS__)

void fab_print_info(struct fab_conn *conn);

int _fab_cq_sread(const char *callf, uint line,
                  struct fid_cq *cq, struct fi_cq_tagged_entry *fi_cqe,
                  size_t count, void *cond, int timeout,
                  struct fi_cq_err_entry *fi_cqerr);

#define fab_cq_sread(...) \
    _fab_cq_sread(__FUNCTION__, __LINE__, __VA_ARGS__)

int _fab_cq_read(const char *callf, uint line,
                 struct fid_cq *cq, struct fi_cq_tagged_entry *fi_cqe,
                 size_t count, struct fi_cq_err_entry *fi_cqerr);

#endif /* _ZHPEQ_UTIL_FAB_H_ */
