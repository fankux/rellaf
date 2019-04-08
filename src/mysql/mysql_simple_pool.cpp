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


#include <strings.h>
#include <deque>
#include "mysql_simple_pool.h"

namespace rellaf {

uint16_t MysqlSimplePool::_s_port;
std::string MysqlSimplePool::_s_host;
std::string MysqlSimplePool::_s_database;
std::string MysqlSimplePool::_s_username;
std::string MysqlSimplePool::_s_password;
std::string MysqlSimplePool::_s_charset;
uint32_t MysqlSimplePool::_s_thread_count;
uint32_t MysqlSimplePool::_s_task_queue_size;
std::deque<MyThread*> MysqlSimplePool::_s_pool;

pthread_mutex_t MysqlSimplePool::_s_tx_lock = PTHREAD_MUTEX_INITIALIZER;
std::map<uint64_t, SqlTx> MysqlSimplePool::_s_tx_pool;

MysqlSimplePool::MysqlSimplePool() : _action_idx(0) {
    if (mysql_library_init(0, nullptr, nullptr) != 0) {
        RELLAF_DEBUG("init mysql lib failed");
        exit(-1);
    }
}

MysqlSimplePool::~MysqlSimplePool() {
    mysql_thread_end();
}

MysqlSimplePool& MysqlSimplePool::instance() {
    static MysqlSimplePool mysql_acc;
    return mysql_acc;
}

void MysqlSimplePool::stop() {
    for (uint32_t i = 0; i < MysqlSimplePool::_s_thread_count; ++i) {
        _s_pool[i]->status = 0;

        if (_s_pool[i]->tid != 0) {
            pthread_join(_s_pool[i]->tid, nullptr);
        }

        delete _s_pool[i]->tasks;

        _s_pool[i]->tasks = nullptr;
        _s_pool[i]->inst = nullptr;

        delete _s_pool[i];
        _s_pool[i] = nullptr;
    }
}

bool MysqlSimplePool::init(const std::string& host, uint16_t port, const std::string& username,
        const std::string& password, const std::string& database, const std::string& charset,
        uint32_t thread_count, uint32_t task_queue_size) {

    MysqlSimplePool::_s_host = host;
    MysqlSimplePool::_s_port = port;
    MysqlSimplePool::_s_database = database;
    MysqlSimplePool::_s_username = username;
    MysqlSimplePool::_s_password = password;
    MysqlSimplePool::_s_charset = charset;
    MysqlSimplePool::_s_thread_count = thread_count;
    MysqlSimplePool::_s_task_queue_size = task_queue_size;

    for (uint32_t i = 0; i < MysqlSimplePool::_s_thread_count; ++i) {
        auto* thread = new(std::nothrow) MyThread;
        if (thread == nullptr) {
            stop();
            return false;
        }

        thread->status = 1;
        thread->tid = nullptr;
        thread->inst = this;
        thread->tasks = new(std::nothrow) Queue<MyContext*>(MysqlSimplePool::_s_task_queue_size,
                true);
        if (thread->tasks == nullptr) {
            delete thread->tasks;
            delete thread;
            stop();
            return false;
        }

        if (pthread_create(&thread->tid, nullptr, MysqlSimplePool::thd_routine, thread) != 0) {
            stop();
            return false;
        }

//        while (thread->status != 2 && thread->status != 0) {
//            RELLAF_DEBUG("pending for sql thread");
//            sleep(1);
//        }

        _s_pool.push_back(thread);
    }

    return true;
}

bool MysqlSimplePool::connect(MYSQL* mysql) {
    MYSQL* tmp = mysql_real_connect(mysql, MysqlSimplePool::_s_host.c_str(),
            MysqlSimplePool::_s_username.c_str(), MysqlSimplePool::_s_password.c_str(),
            MysqlSimplePool::_s_database.c_str(), MysqlSimplePool::_s_port, nullptr, 0);
    if (tmp != mysql) {
        RELLAF_DEBUG("connect to mysql failed, error : %s", mysql_error(mysql));
        return false;
    }
    return true;
}

void MysqlSimplePool::close(MYSQL* mysql) {
    mysql_close(mysql);
}

void MysqlSimplePool::retry(MYSQL* mysql) {
    close(mysql);
    connect(mysql);
}

void* MysqlSimplePool::thd_routine(void* ptr) {
    MyThread* arg = (MyThread*)ptr;
    Queue<MyContext*>* tasks = arg->tasks;

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_thread_init();
    MysqlSimplePool::connect(&mysql);

    RELLAF_DEBUG("mysql thread start");

    ListNode<MyContext*>* context = nullptr;
    while (arg->status) {
        arg->status = 2;
        RELLAF_DEBUG("mysql acc task count : %zu", tasks->size());
        int ret_list = tasks->pop_block(&context, 3000);
        if (ret_list == -1) {
            RELLAF_DEBUG("mysql thread worker fatal");
            arg->status = 0;
            break;
        }
        if (ret_list == 1 || context == nullptr) {
            RELLAF_DEBUG("pop task timeout, ret : %d, context : %lx", ret_list,
                    reinterpret_cast<unsigned long>(context));
            continue;
        }

        const std::string& sql = context->_data->sql;

        InnerResult* result = new(std::nothrow) InnerResult;
        if (result == nullptr) {
            RELLAF_DEBUG("exec sql failed, alloc result error");
            context->_data->latch.count_down();
            delete context;
            break;
        }
        result->status = 0;
        result->row_count = 0;
        result->data = nullptr;
        *(context->_data->result) = result;

        if (strncasecmp(sql.c_str(), "BEGIN", sizeof("BEGIN") - 1) == 0) {
            RELLAF_DEBUG("BEGIN");

            if (mysql_autocommit(&mysql, 0) != 0) {
                RELLAF_DEBUG("begin transaction faild : %s", mysql_error(&mysql));
                result->status = -1;
                result->message = mysql_error(&mysql);
            }

        } else if (strncasecmp(sql.c_str(), "COMMIT", sizeof("COMMIT") - 1) == 0) {
            RELLAF_DEBUG("COMMIT");

            if (mysql_commit(&mysql) != 0) {
                RELLAF_DEBUG("commit transaction faild : %s", mysql_error(&mysql));
                result->status = -1;
                result->message = mysql_error(&mysql);
            }

        } else if (strncasecmp(sql.c_str(), "ROLLBACK", sizeof("ROLLBACK") - 1) == 0) {
            RELLAF_DEBUG("ROLLBACK");

            if (mysql_rollback(&mysql) != 0) {
                RELLAF_DEBUG("rollback transaction faild : %s", mysql_error(&mysql));
                result->status = -1;
                result->message = mysql_error(&mysql);
            }

        } else {
            RELLAF_DEBUG("%s", sql.substr(0, 8).c_str());

            int count = 3;
            while (count) {
                result->status = 0;
                if (mysql_real_query(&mysql, sql.c_str(), sql.size()) == 0) {
                    if (strncasecmp(sql.c_str(), "INSERT", sizeof("INSERT") - 1) == 0) {
                        uint64_t keyid = mysql_insert_id(&mysql);
                        result->data = (void*)keyid;
                    }
                    result->row_count = (int)mysql_affected_rows(&mysql);
                    if (strncasecmp(sql.c_str(), "SELECT", sizeof("SELECT") - 1) == 0) {
                        result->data = (void*)mysql_store_result(&mysql);
                    }
                    break;
                }
                result->status = -1;
                result->message = mysql_error(&mysql);
                RELLAF_DEBUG("exec sql: %s failed, error : %s", sql.c_str(), mysql_error(&mysql));
                --count;
                MysqlSimplePool::retry(&mysql);
            }
        }

        context->_data->latch.count_down();
        delete context;
        context = nullptr;
    }

    RELLAF_DEBUG("mysql thread end");
    MysqlSimplePool::close(&mysql);
    mysql_thread_end();
    return (void*)nullptr;
}

Queue<MyContext*>* MysqlSimplePool::fetch_thread() {
    uint32_t idx = RELLAF_ATOMIC_INC(_action_idx) % MysqlSimplePool::_s_thread_count;
    RELLAF_DEBUG("mysql thread %u fetched, tid : %x", idx, MysqlSimplePool::_s_pool[idx]->tid);
    return MysqlSimplePool::_s_pool[idx]->tasks;
}

MyThread* MysqlSimplePool::new_thread() {
    MyThread* thread = new(std::nothrow) MyThread;
    if (thread == nullptr) {
        return nullptr;
    }

    thread->status = 1;
    thread->tid = 0;
    thread->inst = this;
    thread->tasks = new(std::nothrow) Queue<MyContext*>(MysqlSimplePool::_s_task_queue_size, true);
    if (thread->tasks == nullptr) {
        delete thread;
        return nullptr;
    }

    if (pthread_create(&thread->tid, nullptr, MysqlSimplePool::thd_routine, thread) != 0) {
        delete thread->tasks;
        delete thread;
        return nullptr;
    }

//    while (thread->status != 2 && thread->status != 0) {
//        RELLAF_DEBUG("pending for sql thread");
//        sleep(1);
//    }

    return thread;
}

bool MysqlSimplePool::begin(SqlTx& tx) {
    uint64_t tx_id = RELLAF_ATOMIC_INC(_action_idx);
    RELLAF_DEBUG("mysql transaction begin : %llu, current tx_pool size : %zu", tx_id,
            _s_tx_pool.size());

    MyThread* thread = new_thread();
    if (thread == nullptr) {
        RELLAF_DEBUG("fetch mysql thread failed, tx_id : %llu", tx_id);
        return false;
    }

    InnerResult* result = nullptr;
    MyContext context;
    context.sql = "BEGIN";
    context.result = &result;

    thread->tasks->add_block(&context);
    RELLAF_DEBUG("tx latch waiting, txid : %llu", tx_id);
    context.latch.wait();
    RELLAF_DEBUG("tx latch wait done, txid : %llu", tx_id);

    tx.thread = thread;
    tx.tx_id = tx_id;
    pthread_mutex_lock(&_s_tx_lock);
    _s_tx_pool.insert(std::make_pair(tx.tx_id, tx));
    pthread_mutex_unlock(&_s_tx_lock);

    bool re = result->status == 0;
    delete result;
    return re;
}

bool MysqlSimplePool::tx_end(SqlTx& tx, const std::string& sql) {
    RELLAF_DEBUG("mysql transaction %s : %llu, current tx_pool size : %zu",
            sql.c_str(), tx.tx_id, _s_tx_pool.size());

    pthread_mutex_lock(&_s_tx_lock);
    auto entry = _s_tx_pool.find(tx.tx_id);
    pthread_mutex_unlock(&_s_tx_lock);
    if (entry == _s_tx_pool.end()) {
        RELLAF_DEBUG("no transaction, tx_id : %llu", tx.tx_id);
        return false;
    }
    MyThread* thread = entry->second.thread;

    InnerResult* result = nullptr;
    MyContext context;
    context.sql = sql;
    context.result = &result;
    thread->tasks->add_block(&context);
    RELLAF_DEBUG("tx latch waiting, txid : %u", tx.tx_id);
    context.latch.wait();
    RELLAF_DEBUG("tx latch wait done, txid : %u", tx.tx_id);

    RELLAF_DEBUG("mysql transaction %s, join thread start, tx_id : %llu", sql.c_str(), tx.tx_id);
    thread->status = 0;
    thread->tasks->clear();
    pthread_join(thread->tid, nullptr);
    delete thread->tasks;
    delete thread;
    RELLAF_DEBUG("mysql transaction %s, join thread end, tx_id : %llu", sql.c_str(), tx.tx_id);

    pthread_mutex_lock(&_s_tx_lock);
    _s_tx_pool.erase(tx.tx_id);
    pthread_mutex_unlock(&_s_tx_lock);

    bool re = result->status == 0;
    delete result;
    return re;
}

bool MysqlSimplePool::commit(SqlTx& tx) {
    return tx_end(tx, "COMMIT");
}

bool MysqlSimplePool::rollback(SqlTx& tx) {
    return tx_end(tx, "ROLLBACK");
}

void MysqlSimplePool::tx_execute(SqlTx* tx, const std::string& sql, InnerResult** result_ptr) {
    pthread_mutex_lock(&_s_tx_lock);
    auto entry = _s_tx_pool.find(tx->tx_id);
    pthread_mutex_unlock(&_s_tx_lock);
    if (entry == _s_tx_pool.end()) {
        RELLAF_DEBUG("no transaction, tx_id : %llu", tx->tx_id);
        return;
    }
    MyThread* thread = entry->second.thread;

    MyContext context;
    context.sql = sql;
    context.result = result_ptr;

    int ret = thread->tasks->add_block(&context);
    if (ret != 0) {
        RELLAF_DEBUG("add task failed, ret : %d", ret);
        *result_ptr = nullptr;
        return;
    }
    RELLAF_DEBUG("tx latch waiting, txid : %u", tx->tx_id);
//    FM_NOTICE("tx sql : %s", sql.c_str());
    context.latch.wait();
    RELLAF_DEBUG("tx latch wait done, txid : %u", tx->tx_id);
}

void MysqlSimplePool::execute(const std::string& sql, InnerResult** result_ptr) {
    auto tasks = fetch_thread();
    if (tasks == nullptr) {
        RELLAF_DEBUG("fetch mysql thread failed");
        return;
    }
    MyContext context;
    context.sql = sql;
    context.result = result_ptr;

    int ret = tasks->add_block(&context);
    if (ret != 0) {
        RELLAF_DEBUG("add task failed, ret : %d", ret);
        *result_ptr = nullptr;
        return;
    }
    RELLAF_DEBUG("latch waiting");
    context.latch.wait();
    RELLAF_DEBUG("latch wait done");
}

////////////////// sql executor API //////////////////
SqlResult* MysqlSimplePool::select(const std::string& sql) {
    MyResult* res = new(std::nothrow) MyResult;
    if (res == nullptr) {
        return nullptr;
    }
    if (!select(sql, *res, nullptr)) {
        delete res;
        return nullptr;
    }
    return res;
}

int MysqlSimplePool::execute(const std::string& sql) {
    return insert(sql, nullptr);
}

////////////////// transactional /////////////////////
int MysqlSimplePool::select(const std::string& sql, MyResult& res, SqlTx* tx) {
    InnerResult* result = nullptr;
    if (tx != nullptr) {
        tx_execute(tx, sql, &result);
    } else {
        execute(sql, &result);
    }
    if (result == nullptr) {
        RELLAF_DEBUG("excute sql failed");
        return -1;
    }


    MYSQL_RES* mysql_res = static_cast<MYSQL_RES*>(result->data);
    if (!res.init(mysql_res)) {
        return -1;
    }

    int status = result->status;
    delete result;
    if (status != 0) {
        mysql_free_result(mysql_res);
        return -1;
    }
    return (int)(res.row_count());
}

int MysqlSimplePool::insert(const std::string& sql, uint64_t& keyid, SqlTx* tx) {
    InnerResult* result = nullptr;
    if (tx != nullptr) {
        tx_execute(tx, sql, &result);
    } else {
        execute(sql, &result);
    }
    if (result == nullptr) {
        RELLAF_DEBUG("excute sql failed");
        return -1;
    }

    keyid = reinterpret_cast<uint64_t>(result->data);
    int row_count = result->row_count;

    int status = result->status;
    delete result;
    return status == 0 ? row_count : -1;
}

int MysqlSimplePool::insert(const std::string& sql, SqlTx* tx) {
    InnerResult* result = nullptr;
    if (tx != nullptr) {
        tx_execute(tx, sql, &result);
    } else {
        execute(sql, &result);
    }
    if (result == nullptr) {
        RELLAF_DEBUG("excute sql failed");
        return -1;
    }

    int row_count = result->row_count;

    int status = result->status;
    delete result;
    return status == 0 ? row_count : -1;
}

int MysqlSimplePool::update(const std::string& sql, SqlTx* tx) {
    InnerResult* result = nullptr;
    if (tx != nullptr) {
        tx_execute(tx, sql, &result);
    } else {
        execute(sql, &result);
    }
    if (result == nullptr) {
        RELLAF_DEBUG("excute sql failed");
        return -1;
    }

    int row_count = result->row_count;

    int status = result->status;
    delete result;
    return status == 0 ? row_count : -1;
}

int MysqlSimplePool::del(const std::string& sql, SqlTx* tx) {
    InnerResult* result = nullptr;
    if (tx != nullptr) {
        tx_execute(tx, sql, &result);
    } else {
        execute(sql, &result);
    }
    if (result == nullptr) {
        RELLAF_DEBUG("excute sql failed");
        return -1;
    }

    int row_count = result->row_count;

    int status = result->status;
    delete result;
    return status == 0 ? row_count : -1;
}

////////////////// result set /////////////////////
MyTxEx::MyTxEx() {
    if (RELLAF_ATOMIC_CAS(_init, false, true)) {
        _init = _acc.begin(_tx);
    }
}

MyTxEx::~MyTxEx() {
    rollback();
}

bool MyTxEx::rollback() {
    if (!_init) {
        return false;
    }

    if (RELLAF_ATOMIC_CAS(_is_begin, true, false)) {
        return _acc.rollback(_tx);
    }
    return true;
}

bool MyTxEx::commit(int* done_ret) {
    if (!_init) {
        return false;
    }
    if (RELLAF_ATOMIC_CAS(_is_begin, true, false)) {
        if (!_acc.commit(_tx)) {
            _is_begin = false;
            return false;
        }

        if (_done) {
            int ret = _done();
            if (done_ret != nullptr)
                *done_ret = ret;
        }
    }
    return true;
}

}
