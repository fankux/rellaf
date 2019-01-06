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

#include <assert.h>
#include <string>
#include <map>
#include <functional>

#include "mysql.h"

#include "fqueue.hpp"
#include "flatch.hpp"
#include "cast.hpp"
#include "common.h"

#include "mysql/sql_executor.h"

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

class MyTxEx;

class MysqlSimplePool : public SqlExecutor {
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


    ////////////////// sql executor API //////////////////
    int select(const std::string& sql, SqlResult& res) override;

    int insert(const std::string& sql) override;

    int update(const std::string& sql) override;

    int del(const std::string& sql) override;

    ////////////////// transactional /////////////////////
    int select(const std::string& sql, SqlResult& res, SqlTx* tx);

    int insert(const std::string& sql, SqlTx* tx);

    int insert(const std::string& sql, uint64_t& keyid, SqlTx* tx = nullptr);

    int update(const std::string& sql, SqlTx* tx);

    int del(const std::string& sql, SqlTx* tx);

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
