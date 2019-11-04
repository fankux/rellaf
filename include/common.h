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

#include <pthread.h>
#include "log.h"

namespace rellaf {

#define RELLAF_UNUSED(x) (void)(x)

#define RELLAF_ATOMIC_CAS(_it_, _old_, _new_) __sync_bool_compare_and_swap(&_it_, _old_, _new_)

#define RELLAF_ATOMIC_INC(_it_) __sync_add_and_fetch(&_it_, 1)

#define RELLAF_ATOMIC_DEC(_it_) __sync_add_and_fetch(&_it_, -1)

#define rellaf_singleton(__clazz__)                     \
public:                                                 \
    static __clazz__& instance() {                      \
        static __clazz__ __clazz__##_singleton;         \
        return __clazz__##_singleton;                   \
    }                                                   \
private:                                                \
    __clazz__() {}                                      \

#define RELLAF_AVOID_COPY(__clazz__)                    \
private:                                                \
    __clazz__(const __clazz__&) = delete;               \
    __clazz__& operator=(const __clazz__&) = delete;    \

#define RELLAF_DEFMOVE(__clazz__)                       \
public:                                                 \
    __clazz__() = default;                              \
    __clazz__(const __clazz__& o) = default;            \
    __clazz__(__clazz__&& o) noexcept = default;                 \
    __clazz__& operator=(const __clazz__& o) = default; \
    __clazz__& operator=(__clazz__&& o) noexcept = default;      \

#define RELLAF_DEFMOVE_NO_CTOR(__clazz__)               \
public:                                                 \
    __clazz__(const __clazz__& o) = default;            \
    __clazz__(__clazz__&& o) noexcept = default;                 \
    __clazz__& operator=(const __clazz__& o) = default; \
    __clazz__& operator=(__clazz__&& o) noexcept = default;      \

#ifdef NDEBUG
#define RELLAF_DEBUG(fmt, arg...)
#else
#ifndef RELLAF_DEBUG
#define RELLAF_DEBUG(_fmt_, args...) do {                                   \
    printf("DEBUG [%s:%d] " _fmt_ "\n", __FILE__, __LINE__, ##args);        \
} while (0)
#endif
#endif

}

