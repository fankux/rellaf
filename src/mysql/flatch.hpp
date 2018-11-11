// Copyright 2018 Fankux
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: Fankux (fankux@gmail.com)
//
// single latch signal one thread

#pragma once

#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include "common.h"

namespace rellaf {

class Latch {
public:
    explicit Latch(const uint32_t count = 1) : _count(count) {}

    int wait(const uint32_t timeout_mills = 0) {
        pthread_mutex_lock(&_mutex);

        if (timeout_mills == 0) {
            while (!_signal) {
                pthread_cond_wait(&_cond, &_mutex);
            }
        } else {
            struct timespec tspec;
            clock_gettime(CLOCK_REALTIME, &tspec);
            tspec.tv_sec += timeout_mills / 1000;

            while (!_signal) {
                if (pthread_cond_timedwait(&_cond, &_mutex, &tspec) == ETIMEDOUT) {
                    pthread_mutex_unlock(&_mutex);
                    _signal = false;
                    return 1;
                }
            }
        }

        pthread_mutex_unlock(&_mutex);

        return 0;
    }

    void count_down() {
        if (RELLAF_ATOMIC_DEC(_count) == 0) {
            pthread_mutex_lock(&_mutex);
            _signal = true;
            pthread_cond_signal(&_cond);
            pthread_mutex_unlock(&_mutex);
        }
    }

private:
    volatile uint32_t _count;
    bool _signal = false;

    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t _cond = PTHREAD_COND_INITIALIZER;
};
}
