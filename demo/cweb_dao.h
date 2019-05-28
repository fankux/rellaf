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

#include "sql_builder.h"
#include "info.h"

namespace rellaf {

class CWebDao : public SqlBuilder {
rellaf_singleton(CWebDao);

rellaf_sql_insert_retid(add_info, "INSERT INTO info (content, create_time, update_time) VALUES "
                                  "(#{content}, NOW(), NOW())");

rellaf_sql_select_list(query_infos, "SELECT id, content, create_time, update_time LIMIT #{start}, #{limit}", Info);

rellaf_sql_delete(delete_info, "DELETE FROM info WHERE id=#{id}");

rellaf_sql_update(update_info, "UPDATE info SET content=#{content} AND update_time=NOW() WHERE id=#{id}");

};

}
