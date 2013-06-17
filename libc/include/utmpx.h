/*
 * Copyright (C) 2013 olibc developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _UTMPX_H_
#define _UTMPX_H_

#include <sys/cdefs.h>
#include <utmp.h>
#include <paths.h>

#define _PATH_UTMPX    _PATH_UTMP
#define _PATH_WTMPX    _PATH_WTMP
#define UTMPX_FILE     _PATH_UTMPX
#define UTMPX_FILENAME _PATH_UTMPX
#define WTMPX_FILE     _PATH_WTMPX
#define WTMPX_FILENAME _PATH_WTMPX
#define __UT_LINESIZE   32
#define __UT_NAMESIZE   32
#define __UT_HOSTSIZE   256

__BEGIN_DECLS

struct __exit_status {
  short int e_termination;
  short int e_exit;
};

struct utmpx {
  short int ut_type;
  __pid_t ut_pid;
  char ut_line[__UT_LINESIZE];
  char ut_id[4];
  char ut_user[__UT_NAMESIZE];
  char ut_host[__UT_HOSTSIZE];
  struct __exit_status ut_exit;

  long int ut_session;
  struct timeval ut_tv;
  __int32_t ut_addr_v6[4];
  char ___unused[20];
};

extern void setutxent (void);
extern void endutxent (void);
extern struct utmpx *getutxent (void);
extern struct utmpx *getutxid (const struct utmpx *);
extern struct utmpx *getutxline (const struct utmpx *);
extern struct utmpx *pututxline (const struct utmpx *);
extern int utmpxname (const char *);

extern void updwtmpx (const char *,
                      const struct utmpx *);
extern void getutmp (const struct utmpx *,
                     struct utmp *);
extern void getutmpx (const struct utmp *,
                      struct utmpx *);

__END_DECLS

#endif /* _UTMPX_H_ */
