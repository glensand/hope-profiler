/* Copyright (C) 2024 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/hope-profiler
 */

#pragma once

#include <cstdint>

#ifdef __linux__
#include <time.h>
#endif

namespace hope::profiler {

    class clock final {
    public:
        clock();
        double seconds() const;
    private:

#ifdef __linux__
        timespec m_start_time;
#elif defined(__APPLE__)
        uint64_t m_start_time;
#endif
    };

}