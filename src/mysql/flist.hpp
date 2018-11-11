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

namespace rellaf {

template<class T>
class ListNode {
public:
    virtual ~ListNode() = default;

    ListNode* _prev;
    ListNode* _next;
    T _data;
};

template<class T>
class List {

public:
    List() {
        _max_len = 0;
        _len = 0;
        _head = NULL;
        _tail = NULL;
    }

    explicit List(const size_t max_len) {
        _max_len = max_len;
        _len = 0;
        _head = NULL;
        _tail = NULL;
    }

    virtual ~List() {
        clear();
    }

    inline void clear() {
        ListNode<T>* p = _head;
        ListNode<T>* q = p;
        for (size_t i = _len; i > 0; --i) {
            p = p->_next;
            free(q);
            q = p;
        }
        _len = 0;
    }

    inline size_t size() {
        return _len;
    }

    inline bool isfull() {
        return _max_len != 0 && _len >= _max_len;
    }

    inline bool isempty() {
        return _len == 0;
    }

    inline bool add_tail(T data) {
        if (isfull()) {
            return false;
        }

        ListNode<T>* node = new(std::nothrow) ListNode<T>();
        if (node == NULL) {
            return false;
        }

        node->_data = data;

        if (isempty()) {
            node->_prev = node->_next = NULL;
            _tail = node;
            _head = _tail;
        } else {
            node->_next = NULL;
            node->_prev = _tail;
            _tail->_next = node;
            _tail = _tail->_next;
        }
        ++_len;

        return true;
    }

    inline bool add_tail(ListNode<T>* node) {
        if (isfull()) {
            return false;
        }

        if (isempty()) {
            node->_prev = node->_next = NULL;
            _tail = node;
            _head = _tail;
        } else {
            node->_next = NULL;
            node->_prev = _tail;
            _tail->_next = node;
            _tail = _tail->_next;
        }
        ++_len;

        return true;
    }

    inline ListNode<T>* pop_head() {
        if (isempty()) {
            return NULL;
        }

        ListNode<T>* p = NULL;
        if (_len == 1) { /* last one node */
            p = _head;
            _head = NULL;
            _tail = NULL;
        } else {
            p = _head;
            _head = _head->_next;
            _head->_prev = NULL;

        }
        --_len;
        return p;
    }

    inline void detach(ListNode<T>* p) {
        if (_len == 1) {
            _head = NULL;
            _tail = NULL;
        } else {
            if (p->_prev == NULL) {
                _head = p->_next;
                p->_next->_prev = NULL;
            } else if (p->_next == NULL) {
                _tail = p->_prev;
                p->_prev->_next = NULL;
            } else {
                p->_next->_prev = p->_prev;
                p->_prev->_next = p->_next;
            }
        }
        --_len;
    }

    inline ListNode<T>* get_head() {
        return _head;
    }

    inline ListNode<T>* get_tail() {
        return _tail;
    }

private:
    size_t _max_len;
    size_t _len;
    ListNode<T>* _head;
    ListNode<T>* _tail;
};

struct LruItem {
    std::string key;
};

}
