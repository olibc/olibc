#ifndef _RESOLV_STATIC_H
#define _RESOLV_STATIC_H

#include <netdb.h>

/* this structure contains all the variables that were declared
 * 'static' in the original NetBSD resolver code.
 *
 * this caused vast amounts of crashes and memory corruptions
 * when the resolver was being used by multiple threads.
 *
 * (note: the OpenBSD/FreeBSD resolver has similar 'issues')
 */


extern res_static __res_get_static(void);

#endif /* _RESOLV_STATIC_H */
