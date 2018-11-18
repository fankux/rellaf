/* Copyright (c) 2000, 2016, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

//
// local implmentation of mysql_real_escapte_string
// utf8 and gbk ONLY, source code from mysql5.7
//

#pragma once

#include <string>
#include <string.h>

namespace rellaf {

typedef size_t (* ismbchar_func)(const char* p, const char* e);

typedef size_t (* mbcharlen_func)(unsigned char c);

#define isgbkhead(c) (0x81<=(unsigned char)(c) && (unsigned char)(c)<=0xfe)
#define isgbktail(c) ((0x40<=(unsigned char)(c) && (unsigned char)(c)<=0x7e) || \
                          (0x80<=(unsigned char)(c) && (unsigned char)(c)<=0xfe))

class SqlEscape {
public:
    static SqlEscape& instance(const std::string& encode) {
        static SqlEscape sqlescape(encode);
        return sqlescape;
    }

    /**
     * @brief escape string by quote '\''
     *
     * @param from   [in]  String to escape.
     * @param to     [out] Escaped string output buffer. space MUST BE enough ensured by caller

     *  @return Result value.
     *   @retval != (ulong)-1 Succeeded. Number of bytes written to the output
     *              buffer without the '\0' character.
     *   @retval (ulong)-1    Failed. Use mysql_error() to get error message.
     */
    size_t escape_field(const std::string& from, char* to);

    /**
     * auto detect string length version
     * TODO... test
     */
    bool escape_field(const std::string& field, std::string& out);

    size_t escape_field_quote(const std::string& from, char quote, char* to);


private:
    explicit SqlEscape(const std::string& encode) {
        if (strcasecmp(encode.c_str(), "GBK") == 0) {
            _ismbchar_func = SqlEscape::ismbchar_gbk;
            _mbcharlen_func = SqlEscape::mbcharlen_gbk;
        } else if (strcasecmp(encode.c_str(), "UTF8") == 0) {
            _ismbchar_func = SqlEscape::ismbchar_utf8;
            _mbcharlen_func = SqlEscape::mbcharlen_utf8;
        } else {
            exit(-1);
        }
    }

    size_t escape_string_for_mysql(char* to, size_t to_length, const char* from,
            size_t length);

    size_t escape_quotes_for_mysql(char* to, size_t to_length, const char* from,
            size_t length, char quote);

    size_t my_mbcharlen_ptr(const char* s, const char* e);

    ///////////////////////////// GBK /////////////////////////////////
    static inline size_t ismbchar_gbk(const char* p, const char* e) {
        return (isgbkhead(*(p)) && (e) - (p) > 1 && isgbktail(*((p) + 1)) ? 2 : 0);
    }

    static inline size_t mbcharlen_gbk(unsigned char c) {
        return (isgbkhead(c) ? 2 : 1);
    }

    ///////////////////////////// UTF8 /////////////////////////////////
    static inline size_t ismbchar_utf8(const char* b, const char* e) {
        int res = valid_mbcharlen_utf8((const unsigned char*)b, (const unsigned char*)e);
        return (size_t)((res > 1) ? res : 0);
    }

    static inline size_t mbcharlen_utf8(unsigned char c) {
        if (c < 0x80) {
            return 1;
        } else if (c < 0xc2) {
            return 0; /* Illegal mb head */
        } else if (c < 0xe0) {
            return 2;
        } else if (c < 0xf0) {
            return 3;
        }
        return 0; /* Illegal mb head */;
    }

    static int valid_mbcharlen_utf8(const unsigned char* s, const unsigned char* e);

    static int valid_mbcharlen_utf8mb3(const unsigned char* s, const unsigned char* e);

private:
    ismbchar_func _ismbchar_func = nullptr;
    mbcharlen_func _mbcharlen_func = nullptr;
};

}
