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

#include "mysql_escape.h"

namespace rellaf {

static const int MY_CS_ILSEQ = 0;     /* Wrong by sequence: wb_wc                   */
static const int MY_CS_TOOSMALL = (-101);  /* Need at least one byte:    wc_mb and mb_wc */
static const int MY_CS_TOOSMALL2 = (-102);  /* Need at least two bytes:   wc_mb and mb_wc */
static const int MY_CS_TOOSMALL3 = (-103);  /* Need at least three bytes: wc_mb and mb_wc */

#define IS_CONTINUATION_BYTE(c) (((c) ^ 0x80) < 0x40)

/**
  Escapes special characters in a string for use in an SQL statement.

  Escapes special characters in the unescaped string, taking into account
  the current character set and sql mode of the connection so that is safe
  to place the string in a mysql_query(). This function must be used for
  binary data.

  This function does not work correctly when
  string quote in the SQL statement is different than '\''.

  @deprecated This function should not be used.
              Use mysql_real_escape_string_quote instead.

  @see mysql_real_escape_string_quote

  @param to     [out] Escaped string output buffer.
  @param from   [in]  String to escape.
  @param length [in]  String to escape length.

  @return Result value.
    @retval != (ulong)-1 Succeeded. Number of bytes written to the output
                         buffer without the '\0' character.
    @retval (ulong)-1    Failed. Use mysql_error() to get error message.
*/
size_t SqlEscape::escape_field(const std::string& from, char* to) {
    return escape_field_quote(from, '\'', to);
}

/**
  Escapes special characters in a string for use in an SQL statement.

  Escapes special characters in the unescaped string, taking into account
  the current character set and sql mode of the connection so that is safe
  to place the string in a mysql_query(). This function must be used for
  binary data.

  This function should be used for escaping identifiers and string parameters.

  @param mysql  [in]  MySQL connection structure. // deprecate
  @param to     [out] Escaped string output buffer.
  @param from   [in]  String to escape.
  @param quote  [in]  String quoting character used in an SQL statement. This
                      should be one of '\'', '"' or '`' depending on the
                      parameter quoting applied in the SQL statement.

  @return Result value.
    @retval != (ulong)-1 Succeeded. Number of bytes written to the output
                         buffer without the '\0' character.
    @retval (ulong)-1    Failed.
*/
size_t SqlEscape::escape_field_quote(const std::string& from, char quote, char* to) {
    if (quote == '`') {
        return escape_quotes_for_mysql(to, 0, from.c_str(), from.size(), quote);
    }
    return escape_string_for_mysql(to, 0, from.c_str(), from.size());
}

size_t SqlEscape::escape_string_for_mysql(char* to, size_t to_length, const char* from,
        size_t length) {
    const char* to_start = to;
    const char* end = nullptr;
    const char* to_end = to_start + (to_length ? to_length - 1 : 2 * length);
    bool overflow = false;
    for (end = from + length; from < end; from++) {
        char escape = 0;
        size_t tmp_length = 0;
        if ((tmp_length = _ismbchar_func(from, end))) {
            if (to + tmp_length > to_end) {
                overflow = true;
                break;
            }
            while (tmp_length--) {
                *to++ = *from++;
            }
            from--;
            continue;
        }
        /*
         If the next character appears to begin a multi-byte character, we
         escape that first byte of that apparent multi-byte character. (The
         character just looks like a multi-byte character -- if it were actually
         a multi-byte character, it would have been passed through in the test
         above.)

         Without this check, we can create a problem by converting an invalid
         multi-byte character into a valid one. For example, 0xbf27 is not
         a valid GBK character, but 0xbf5c is. (0x27 = ', 0x5c = \)
        */
        tmp_length = my_mbcharlen_ptr(from, end);
        if (tmp_length > 1) {
            escape = *from;
        } else {
            switch (*from) {
                case 0:                /* Must be escaped for 'mysql' */
                    escape = '0';
                    break;
                case '\n':                /* Must be escaped for logs */
                    escape = 'n';
                    break;
                case '\r':
                    escape = 'r';
                    break;
                case '\\':
                    escape = '\\';
                    break;
                case '\'':
                    escape = '\'';
                    break;
                case '"':                /* Better safe than sorry */
                    escape = '"';
                    break;
                case '\032':            /* This gives problems on Win32 */
                    escape = 'Z';
                    break;
                default:
                    break;
            }
        }
        if (escape) {
            if (to + 2 > to_end) {
                overflow = true;
                break;
            }
            *to++ = '\\';
            *to++ = escape;
        } else {
            if (to + 1 > to_end) {
                overflow = true;
                break;
            }
            *to++ = *from;
        }
    }
    *to = 0;
    return overflow ? (size_t) -1 : (size_t) (to - to_start);
}

/**
  Escape string with backslashes (\)

  SYNOPSIS
    escape_string_for_mysql()
    to                  Buffer for escaped string
    to_length           Length of destination buffer, or 0
    from                The string to escape
    length              The length of the string to escape

  DESCRIPTION
    This escapes the contents of a string by adding backslashes before special
    characters, and turning others into specific escape sequences, such as
    turning newlines into \n and null bytes into \0.

  NOTE
    To maintain compatibility with the old C API, to_length may be 0 to mean
    "big enough"

  RETURN VALUES
    (size_t) -1 The escaped string did not fit in the to buffer
    #           The length of the escaped string
*/
size_t SqlEscape::escape_quotes_for_mysql(char* to, size_t to_length, const char* from,
        size_t length, char quote) {
    const char* to_start = to;
    const char* end = nullptr;
    const char* to_end = to_start + (to_length ? to_length - 1 : 2 * length);
    bool overflow = false;
    for (end = from + length; from < end; from++) {
        size_t tmp_length = 0;
        if ((tmp_length = _ismbchar_func(from, end))) {
            if (to + tmp_length > to_end) {
                overflow = true;
                break;
            }
            while (tmp_length--) {
                *to++ = *from++;
            }
            from--;
            continue;
        }
        /*
          We don't have the same issue here with a non-multi-byte character being
          turned into a multi-byte character by the addition of an escaping
          character, because we are only escaping the ' character with itself.
         */
        if (*from == quote) {
            if (to + 2 > to_end) {
                overflow = true;
                break;
            }
            *to++ = quote;
            *to++ = quote;
        } else {
            if (to + 1 > to_end) {
                overflow = true;
                break;
            }
            *to++ = *from;
        }
    }
    *to = 0;
    return overflow ? (size_t) ~0 : (size_t) (to - to_start);
}

/**
  Get the length of the first code in given sequence of chars.

  [deprecated bellow, GB18030 not support yet]
  This func is introduced because we can't determine the length by
  checking the first byte only for gb18030, so we first try my_mbcharlen,
  and then my_mbcharlen_2 if necessary to get the length

  @param[in]  s    start of the char sequence
  @param[in]  e    end of the char sequence
  @return     The length of the first code, or 0 for invalid code
*/
size_t SqlEscape::my_mbcharlen_ptr(const char* s, const char* e) {
    return _mbcharlen_func((unsigned char) *s);

//    if (len == 0 && my_mbmaxlenlen(cs) == 2 && s + 1 < e) {
//        len = my_mbcharlen_2(cs, (uchar) * s, (uchar) * (s + 1));
//        /* It could be either a valid multi-byte GB18030 code, or invalid
//        gb18030 code if return value is 0 */
//        DBUG_ASSERT(len == 0 || len == 2 || len == 4);
//    }
}

int SqlEscape::valid_mbcharlen_utf8(const unsigned char* s, const unsigned char* e) {
    if (s >= e) {
        return MY_CS_TOOSMALL;
    }

    unsigned char c = s[0];
    if (c < 0xf0) {
        return valid_mbcharlen_utf8mb3(s, e);
    }

    return MY_CS_ILSEQ;
}

int SqlEscape::valid_mbcharlen_utf8mb3(const unsigned char* s, const unsigned char* e) {
    unsigned char c = s[0];
    if (c < 0x80) {
        return 1;
    }

    if (c < 0xc2) {
        return MY_CS_ILSEQ;
    }

    if (c < 0xe0) {
        if (s + 2 > e) { /* We need 2 characters */
            return MY_CS_TOOSMALL2;
        }

        if (!(IS_CONTINUATION_BYTE(s[1]))) {
            return MY_CS_ILSEQ;
        }

        return 2;
    }

    if (s + 3 > e) { /* We need 3 characters */
        return MY_CS_TOOSMALL3;
    }

    if (!(IS_CONTINUATION_BYTE(s[1]) && IS_CONTINUATION_BYTE(s[2]) &&
            (c >= 0xe1 || s[1] >= 0xa0))) {
        return MY_CS_ILSEQ;
    }

    return 3;
}

}