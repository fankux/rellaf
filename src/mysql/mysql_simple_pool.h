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

#include <string>
#include <map>
#include <mysql.h>
#include "boost/lexical_cast.hpp"
#include "fqueue.hpp"
#include "flatch.hpp"
#include "common.h"

namespace rellaf {

struct MyResult {
    int status;
    int row_count;
    std::string message;
    void* data;
};

struct MyContext {
    std::string sql;
    Latch latch;
    MyResult** result;
};

class MysqlSimplePool;

struct MyThread {
    pthread_t tid;
    MysqlSimplePool* inst;
    Queue<MyContext*>* tasks;
    int status;
};

struct SqlTx {
    uint64_t tx_id = 0;
    MyThread* thread = nullptr;
};

class MyRow {
RELLAF_DEFMOVE_NO_CTOR(MyRow)

public:
    MyRow() = default;

    friend class MyRes;

    template<class T=std::string>
    T get(size_t field) {
        assert(field < _field_count);
        return boost::lexical_cast<T>(_row[field]);
    }

    std::string operator[](const size_t idx) {
        return get(idx);
    }

    inline bool isnull() {
        return _row == nullptr;
    }

    inline bool isnotnull() {
        return !isnull();
    }

private:
    MyRow(MYSQL_ROW row, size_t field_count) : _row(row), _field_count(field_count) {};

private:
    MYSQL_ROW _row = nullptr;
    size_t _field_count = 0;
};

class MyField {
RELLAF_DEFMOVE_NO_CTOR(MyField);

public:
    MyField() = default;

    friend class MyRes;

    const std::string& name() const {
        return _name;
    }

    enum_field_types type() const {
        return _type;
    }

private:
    MyField(const std::string& name, enum enum_field_types type) : _name(name), _type(type) {}

private:
    std::string _name;
    enum enum_field_types _type;
};

class MyRes {
RELLAF_AVOID_COPY(MyRes)

public:
    explicit MyRes(MYSQL_RES* res) : _res(res) {}

    virtual ~MyRes();

    void reset();

    void operator()(MYSQL_RES* res);

    bool good();

    size_t row_count();

    size_t field_count();

    MyRow fetch_row();

    void fetch_fields(std::deque<MyField>& fields);

private:
    MYSQL_RES* _res;
};

class MyTxEx;

class MysqlSimplePool {
public:
    virtual ~MysqlSimplePool() {
        mysql_thread_end();
    }

    static MysqlSimplePool& instance() {
        static MysqlSimplePool mysql_acc;
        return mysql_acc;
    }

    void stop();

    bool init(const std::string& host, uint16_t port, const std::string& username,
            const std::string& password, const std::string& database,
            const std::string& charset = "utf8", uint32_t thread_count = 3,
            uint32_t task_queue_size = 10);

    int insert(const std::string& sql, SqlTx* tx = nullptr);

    int insert(const std::string& sql, uint64_t& keyid, SqlTx* tx = nullptr);

    int remove(const std::string& sql, SqlTx* tx = nullptr);

    int update(const std::string& sql, SqlTx* tx = nullptr);

    MYSQL_RES* query(const std::string& sql, SqlTx* tx = nullptr);

    bool begin(SqlTx& tx);

    bool commit(SqlTx& tx);

    bool rollback(SqlTx& tx);

private:
    MysqlSimplePool() : _action_idx(0) {
        if (mysql_library_init(0, nullptr, nullptr) != 0) {
            RELLAF_DEBUG("init mysql lib failed");
            exit(-1);
        }
    }

    void execute(const std::string& sql, MyResult** result_ptr);

    void tx_execute(SqlTx* tx, const std::string& sql, MyResult** result_ptr);

    static bool connect(MYSQL* mysql);

    static void close(MYSQL* mysql);

    static void retry(MYSQL* mysql);

    static void* thd_routine(void*);

    Queue<MyContext*>* fetch_thread();

    MyThread* new_thread();

    bool tx_end(SqlTx& tx, const std::string& sql);

private:
    static uint16_t _s_port;
    static std::string _s_host;
    static std::string _s_database;
    static std::string _s_username;
    static std::string _s_password;
    static std::string _s_charset;
    static uint32_t _s_thread_count;
    static uint32_t _s_task_queue_size;

    static std::deque<MyThread*> _s_pool;

    static pthread_mutex_t _s_tx_lock;
    static std::map<uint64_t, SqlTx> _s_tx_pool; // tx_id ==> sql_tx

    volatile uint64_t _action_idx;

};

class MyTxEx {
RELLAF_AVOID_COPY(MyTxEx)

public:
    MyTxEx();

    virtual ~MyTxEx();

    bool rollback();

    bool commit(int* done_ret = nullptr);

    void set_done(const std::function<int()>& done) {
        _done = done;
    }

private:
    bool _init = false;
    bool _is_begin = false;
    MysqlSimplePool& _acc = MysqlSimplePool::instance();
    std::function<int()> _done;
    SqlTx _tx;
};

}
