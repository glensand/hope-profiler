/* Copyright (C) 2024 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#pragma once

#include "hope-profiler/profiler_manager.h"

namespace hope::profiler {

    extern manager* __glob_manager;

    struct block final {
        block_data data;

        block(const char* name) {
            data.start_sec = __glob_manager->current_time();
            data.block_name = name;
        }

        ~block() {
            data.end_sec = __glob_manager->current_time();
            __glob_manager->upload_block(data);
        }
    };

}

#define HOPE_STR(Name) #Name
#define HOPE_SCOPE(Name) const hope::profiler::block Name (HOPE_STR(Name));
#define HOPE_REG_THREAD(Name) hope::profiler::__glob_manager->register_thread(HOPE_STR(Name));
