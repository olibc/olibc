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

#include <ctype.h>
#include <stdlib.h>

long a64l(const char *str)
{
  char ch;
  long result = 0;
  long val;
  int shift;
  for (shift = 0;
       shift < 36 && *str;
       shift += 6, ++str) {
    ch = *str;
    if (ch >= 'a') {
      val = (ch - 'a') + 38;
    } else if (ch >= 'A') {
      val = (ch - 'A') + 12;
    } else if (ch >= '0') {
      val = (ch - '0') + 2;
    } else if (ch == '/') {
      val = 1;
    } else {
      val = 0;
    }
    result |= val << shift;
  }
  return result;
}
