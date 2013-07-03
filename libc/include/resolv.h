/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef _RESOLV_H_
#define _RESOLV_H_

#include <sys/param.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <netdb.h>

__BEGIN_DECLS


#define MAXALIASES      35
#define MAXADDRS        35

typedef struct res_static {
    char*           h_addr_ptrs[MAXADDRS + 1];
    char*           host_aliases[MAXALIASES];
    char            hostbuf[8*1024];
    u_int32_t       host_addr[16 / sizeof(u_int32_t)];  /* IPv4 or IPv6 */
    FILE*           hostf;
    int             stayopen;
    const char*     servent_ptr;
    struct servent  servent;
    struct hostent  host;
} *res_static;

typedef enum { res_goahead, res_nextns, res_modified, res_done, res_error }
        res_sendhookact;

typedef res_sendhookact (*res_send_qhook)(struct sockaddr * const *,
                                              const u_char **, int *,
                                              u_char *, int, int *);

typedef res_sendhookact (*res_send_rhook)(const struct sockaddr *,
                                              const u_char *, int, u_char *,
                                              int, int *);

/*
 * Global defines and variables for resolver stub.
 */
#define MAXNS                   3       /* max # name servers we'll track */
#define MAXDFLSRCH              3       /* # default domain levels to try */
#define MAXDNSRCH               6       /* max # domains in search path */
#define LOCALDOMAINPARTS        2       /* min levels in name that is "local" */

#define RES_TIMEOUT             5       /* min. seconds between retries */
#define MAXRESOLVSORT           10      /* number of net to sort on */
#define RES_MAXNDOTS            15      /* should reflect bit field size */
#define RES_MAXRETRANS          30      /* only for resolv.conf/RES_OPTIONS */
#define RES_MAXRETRY            5       /* only for resolv.conf/RES_OPTIONS */
#define RES_DFLRETRY            2       /* Default #/tries. */
#define RES_MAXTIME             65535   /* Infinity, in milliseconds. */

struct res_state;

struct __res_state {
        int     retrans;                /* retransmission time interval */
        int     retry;                  /* number of times to retransmit */
#ifdef sun
        u_int   options;                /* option flags - see below. */
#else
        u_long  options;                /* option flags - see below. */
#endif
        int     nscount;                /* number of name servers */
        struct sockaddr_in
                nsaddr_list[MAXNS];     /* address of name server */
#define nsaddr  nsaddr_list[0]          /* for backward compatibility */
        u_short id;                     /* current message id */
        char    *dnsrch[MAXDNSRCH+1];   /* components of domain to search */
        char    defdname[256];          /* default domain (deprecated) */
#ifdef sun
        u_int   pfcode;                 /* RES_PRF_ flags - see below. */
#else
        u_long  pfcode;                 /* RES_PRF_ flags - see below. */
#endif
        unsigned ndots:4;               /* threshold for initial abs. query */
        unsigned nsort:4;               /* number of elements in sort_list[] */
        char    unused[3];
        struct {
                struct in_addr  addr;
                uint32_t        mask;
        } sort_list[MAXRESOLVSORT];
#ifdef __OLD_RES_STATE
        char lookups[4];
#else
        res_send_qhook qhook;           /* query hook */
        res_send_rhook rhook;           /* response hook */
        int     res_h_errno;            /* last one set for this context */
        int     _vcsock;                /* PRIVATE: for res_send VC i/o */
        u_int   _flags;                 /* PRIVATE: see below */
        u_int   _pad;                   /* make _u 64 bit aligned */
        union {
                /* On an 32-bit arch this means 512b total. */
                char    pad[72 - 4*sizeof (int) - 2*sizeof (void *)];
                struct {
                        uint16_t                nscount;
                        uint16_t                nstimes[MAXNS]; /* ms. */
                        int                     nssocks[MAXNS];
                        struct __res_state_ext *ext;    /* extention for IPv6 */
                } _ext;
        } _u;
#endif
        struct res_static   rstatic[1];
};



extern struct __res_state *__res_state(void);
#define _res (*__res_state())

#define b64_ntop __b64_ntop
#define b64_pton __b64_pton
extern int b64_ntop(u_char const*, size_t, char*, size_t);
extern int b64_pton(char const*, u_char*, size_t);

#define dn_comp __dn_comp
extern int dn_comp(const char*, u_char*, int, u_char**, u_char**);
extern int dn_expand(const u_char*, const u_char*, const u_char*, char*, int);

__END_DECLS

#endif /* _RESOLV_H_ */
