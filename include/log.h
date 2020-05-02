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

#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <sstream>
#include <iostream>

namespace rellaf {

class LogStream {
public:
    // TODO.. time
    explicit LogStream(int level, const std::string& file, const int line) :
            _level(level), _file(file), _line(line) {
#ifndef NDEBUG
        if (_level == 0) {
            _pre_str = "D [" + file + ":" + std::to_string(line) + "] ";
            return;
        }
#endif
        if (_level == 1) {
            _pre_str = "I [" + file + ":" + std::to_string(line) + "] ";
        } else if (_level == 2) {
            _pre_str = "W [" + file + ":" + std::to_string(line) + "] ";
        } else if (_level == 3) {
            _pre_str = "E [" + file + ":" + std::to_string(line) + "] ";
        } else if (_level == 4) {
            _pre_str = "F [" + file + ":" + std::to_string(line) + "] ";
        }
    }

    ~LogStream() {
        flush();
    }

    inline LogStream& operator<<(int16_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(int val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(int64_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(uint16_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(uint32_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(uint64_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(float val) {
        _ss << val;
        return *this;
    }

#ifdef __APPLE__

    inline LogStream& operator<<(size_t val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(ssize_t val) {
        _ss << val;
        return *this;
    }

#endif

    inline LogStream& operator<<(double val) {
        _ss << val;
        return *this;
    }

    inline LogStream& operator<<(const std::string& val) {
        _ss << val;
        return *this;
    }

    template<typename T>
    inline LogStream& operator<<(const std::vector<T>& t) {
        iter_list(t);
        return *this;
    }

    template<typename T>
    inline LogStream& operator<<(const std::deque<T>& t) {
        iter_list(t);
        return *this;
    }

#if __cplusplus >= 201703L
    template<typename T>
    inline LogStream& operator<<(const std::list<T>& t) {
        iter_list(t);
        return *this;
    }
#endif

    template<typename T>
    inline LogStream& operator<<(const std::set<T>& t) {
        iter_list(t);
        return *this;
    }

    template<typename T>
    inline LogStream& operator<<(const std::unordered_set<T>& t) {
        iter_list(t);
        return *this;
    }

    template<typename K, typename V>
    inline LogStream& operator<<(const std::map<K, V>& t) {
        iter_kv(t);
        return *this;
    }

    template<typename K, typename V>
    inline LogStream& operator<<(const std::unordered_map<K, V>& t) {
        iter_kv(t);
        return *this;
    }

    template<typename K, typename V>
    inline LogStream& operator<<(const std::pair<K, V>& t) {
        *this << "<" << t.first << "," << t.second << ">";
        return *this;
    }

    template<typename T>
    void iter_list(const T& t) {
        *this << "[";
        size_t i = 0;
        for (auto iter = t.begin(); iter != t.end(); ++iter) {
            *this << *iter;
            if (i++ != t.size() - 1) {
                *this << ",";
            }
        }
        *this << "]";
    }

    template<typename T>
    void iter_kv(const T& t) {
        *this << "{";
        size_t i = 0;
        for (auto iter = t.begin(); iter != t.end(); ++iter) {
            *this << "(" << iter->first << ":" << iter->second << ")";
            if (i++ != t.size() - 1) {
                *this << ",";
            }
        }
        *this << "}";
    }

    void flush() {
        std::cout << _pre_str << _ss.str() << std::endl;
        _ss.clear();
    }

private:
    std::stringstream _ss;
    int _level;
    const std::string& _file;
    const int _line;
    std::string _pre_str;
};

template<typename T>
class LogMessage {
public:
    explicit LogMessage(int level, const std::string& file, const int line) {
        if (_stream == nullptr) {
            _stream = new T(level, file, line);
        }
    }

    ~LogMessage() {
        if (_stream != nullptr) {
            _stream->flush();
        }
    }

    T& stream() {
        return *_stream;
    }

private:
    T* _stream = nullptr;
};

}

#ifdef NDEBUG
#define FLOG_DEBUG
#else
#ifndef FLOG_DEBUG
#define FLOG_DEBUG rellaf::LogMessage<rellaf::LogStream>(0, __FILE__, __LINE__).stream()
#endif
#endif

#define FLOG_INFO rellaf::LogMessage<rellaf::LogStream>(1, __FILE__, __LINE__).stream()
#define FLOG(level)  FLOG_##level
