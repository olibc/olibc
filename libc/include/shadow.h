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
#ifndef _SHADOW_H_
#define _SHADOW_H_

#include <paths.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stddef.h>

__BEGIN_DECLS

struct spwd {
  char *sp_namp;
  char *sp_pwdp;
  long int sp_lstchg;
  long int sp_min;
  long int sp_max;
  long int sp_warn;
  long int sp_inact;
  long int sp_expire;
  unsigned long int sp_flag;
};

extern void setspent (void);
extern void endspent (void);
extern struct spwd *getspent (void);
extern struct spwd *getspnam (const char *name);
extern struct spwd *sgetspent (const char *string);
extern struct spwd *fgetspent (FILE *stream);
extern int putspent (const struct spwd *p, FILE *stream);
extern int getspent_r (struct spwd *result_buf, char *buffer,
                       size_t buflen, struct spwd **result);

extern int getspnam_r (const char *name, struct spwd *result_buf,
                       char *buffer, size_t buflen,
                       struct spwd **result);

extern int sgetspent_r (const char *string, struct spwd *result_buf,
                        char *buffer, size_t buflen,
                        struct spwd **result);

extern int fgetspent_r (FILE *stream, struct spwd *result_buf,
                        char *buffer, size_t buflen,
                        struct spwd **result);

extern int lckpwdf (void);
extern int ulckpwdf (void);

__END_DECLS

#endif /* _SHADOW_H_ */
