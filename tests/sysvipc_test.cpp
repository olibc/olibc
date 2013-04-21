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

#include <gtest/gtest.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

static const key_t key = 0x1234;

TEST(sysvipc, sem) {
  int semid, gsemid;
  struct sembuf sblock = {0  /* sem_num */,
                          -1 /* sem_op  */,
                          0  /* sem_flg */};
  struct sembuf sbunlock = {0  /* sem_num */,
                            1 /* sem_op  */,
                            0  /* sem_flg */};
  semctl(semid, 0, IPC_RMID); /* Prevent stuck by last fault */
  ASSERT_TRUE((semid = semget(key, 1, 0666 | IPC_CREAT)) != -1);
  ASSERT_TRUE(semctl(semid, 0, SETVAL, 1) != -1);

  ASSERT_TRUE((gsemid = semget(key, 1, 0)) != -1);
  ASSERT_EQ(semid, gsemid);

  ASSERT_TRUE(semop(semid, &sblock, 1) != -1);
  ASSERT_TRUE(semop(semid, &sbunlock, 1) != -1);
  ASSERT_TRUE(semctl(semid, 0, IPC_RMID) != -1);
}
