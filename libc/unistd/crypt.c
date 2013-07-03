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

#include <unistd.h>
#include "crypt_private.h"

char *
crypt(const char *key, const char *salt)
{
  if (salt[0] == '$' && salt[2] == '$') {
    switch (salt[1]) {
#ifdef MD5_IMPL
      case '1': /* md5 */
        /* Not implment yet. */
        break;
#endif
      /* olibc don't implment SHA 256/512 yet. */
      case '5': /* SHA 256 */
      case '6': /* SHA 512 */
      default:
        break;
    }
  }
  return _des_crypt(key, salt);
}
