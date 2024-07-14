/* Copyright (C) 2024 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/hope-profiler
 */

#pragma once

#include "third_party/clock.h"

namespace hope::profiler {

    struct block_data final {
        const char* block_name = nullptr;
        double start_sec = 0.0;
        double end_sec = 0.0;
        uint32_t thread_id;
    };

    class manager final {
    public:

        manager();
        ~manager();

        double current_time() const {
            return m_counter.seconds();
        }

        void upload_block(block_data& blk);
        void register_thread(const char* name);

    private:
        void store_proc();

        clock m_counter;
    };
    
}
