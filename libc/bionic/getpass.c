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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pty.h>

char *getpass(const char* prompt) {
  struct termios backup_tios, new_tios;
  static char buf[PASS_MAX+1];
  int out, in;
  int pass_len = 0;
  char cbuf;
  int rv;
  in = open("/dev/tty", O_RDWR | O_CLOEXEC);
  if (in < 0){
    in = STDIN_FILENO;
    out = STDOUT_FILENO;
  } else {
    out = in;
  }
  if (tcgetattr(in, &backup_tios) == 0) {
    new_tios = backup_tios;
    new_tios.c_lflag &= ~(ECHO|ISIG);
    tcsetattr(in, TCSAFLUSH, &new_tios);
  }

  write(out,prompt,strlen(prompt));

  while (pass_len < PASS_MAX) {
    rv = TEMP_FAILURE_RETRY(read(in, &cbuf, 1));
    if (rv <= 0 || cbuf == '\n') {
      break;
    } else {
      buf[pass_len++] = cbuf;
    }
  }
  buf[pass_len] = 0;

  tcsetattr(in, TCSAFLUSH, &backup_tios);
  if (in == STDIN_FILENO) {
    close(STDIN_FILENO);
  }
  return buf;
}
