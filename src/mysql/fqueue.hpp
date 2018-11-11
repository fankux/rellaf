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

#pragma once

#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string>
#include "flist.hpp"

namespace rellaf {

const int QUEUE_ETIMEOUT = 1;
const int QUEUE_OK = 0;
const int QUEUE_FAIL = -1;

template <class T>
class Queue {

public:
    virtual ~Queue() {}

    Queue(const size_t size, bool blocking) {
        _max_size = size == 0 ? SIZE_MAX : size;
        _blocking = blocking;
        if (_blocking) {
            _mutex = PTHREAD_MUTEX_INITIALIZER;
            _full_cond = PTHREAD_COND_INITIALIZER;
            _empty_cond = PTHREAD_COND_INITIALIZER;
        }
    }

    inline size_t size() {
        return _list.size();
    }

    inline bool isempty() {
        return _list.size() == 0;
    }

    inline bool isfull() {
        return  _list.size() >= _max_size;
    }

    inline int add_block(T val, uint32_t timeout_mills = 0) {
        pthread_mutex_lock(&_mutex);

        while (isfull()) {
            if (timeout_mills == 0) {
                while (!_full_signal) {
                    pthread_cond_wait(&_full_cond, &_mutex);
                }
            } else {
                struct timespec tspec;
                clock_gettime(CLOCK_REALTIME, &tspec);
                tspec.tv_sec += timeout_mills / 1000;
                while (!_full_signal) {
                    if (pthread_cond_timedwait(&_full_cond, &_mutex, &tspec) == ETIMEDOUT) {
                        _full_signal = false;
                        pthread_mutex_unlock(&_mutex);
                        return 1;
                    }
                }
            }
        }
        bool result = _list.add_tail(val);

        _empty_signal = true;
        pthread_cond_signal(&_empty_cond);

        _full_signal = false;
        pthread_mutex_unlock(&_mutex);
        return result ? 0 : -1;
    }

    inline int add(T val, uint32_t timeout_mills = 0) {
        if (_blocking) {
            return add_block(val, timeout_mills);
        }

        if (isfull()) {
            return -1;
        }
        return _list.add_tail(val) ? 0 : -1;
    }

    inline int pop_block(ListNode<T>** n, uint32_t timeout_mills = 0) {
        if (n == NULL) {
            return -1;
        }

        pthread_mutex_lock(&_mutex);
        while (isempty()) {
            if (timeout_mills == 0) {
                while (!_empty_signal) {
                    pthread_cond_wait(&_empty_cond, &_mutex);
                }
            } else {
                struct timespec tspec;
                clock_gettime(CLOCK_REALTIME, &tspec);
                tspec.tv_sec += timeout_mills / 1000;
                while (!_empty_signal) {
                    if (pthread_cond_timedwait(&_empty_cond, &_mutex, &tspec) == ETIMEDOUT) {
                        *n = NULL;
                        _empty_signal = false;
                        pthread_mutex_unlock(&_mutex);
                        return 1;
                    }
                }
            }
        }
        *n = _list.pop_head();

        _full_signal = true;
        pthread_cond_signal(&_full_cond);

        _empty_signal = false;
        pthread_mutex_unlock(&_mutex);
        return 0;
    }

    inline int pop(ListNode<T>** n, uint32_t timeout_mills = 0) {
        if (n == NULL) {
            return -1;
        }
        if (_blocking) {
            return pop_block(n, timeout_mills);
        } else {
            *n = _list.pop_head();
        }
        return 0;
    }

    inline ListNode<T>* peek() {
        return _list.get_head();
    }

    inline void clear() {
        if (_blocking) {
            pthread_mutex_lock(&_mutex);

            _full_signal = true;
            pthread_cond_signal(&_full_cond);
            _empty_signal = true;
            pthread_cond_signal(&_empty_cond);

            pthread_mutex_unlock(&_mutex);
        }
        _full_signal = false;
        _empty_signal = false;
        _list.clear();
    }


private:
    List<T> _list;
    bool _blocking;
    size_t _max_size;

    pthread_mutex_t _mutex;

    bool _full_signal = false;
    bool _empty_signal = false;
    pthread_cond_t _full_cond;
    pthread_cond_t _empty_cond;
};

}
