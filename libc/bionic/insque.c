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

#include <stddef.h>
#include <search.h>

void insque(void *elem, void *prev) {
  struct qelem *qelem = (struct qelem *)elem;
  struct qelem *qprev = (struct qelem *)prev;
  if (qprev == NULL) {
    qelem->q_forw = NULL;
    qelem->q_back = NULL;
  } else {
    struct qelem *qnext = (struct qelem *)qprev->q_forw;
    qprev->q_forw = qelem;
    if (qnext != NULL) {
      qnext->q_back = qelem;
    }
    qelem->q_forw = qnext;
    qelem->q_back = qprev;
  }
}
