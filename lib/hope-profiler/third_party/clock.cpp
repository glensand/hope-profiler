#include "clock.h"

// TODO Windows
#ifdef __linux__
namespace hope::profiler {
    static timespec diff(timespec start, timespec end) {
        timespec temp;
        if ((end.tv_nsec - start.tv_nsec) < 0) {
            temp.tv_sec = end.tv_sec - start.tv_sec - 1;
            temp.tv_nsec = 1e9 + end.tv_nsec - start.tv_nsec;
        } else {
            temp.tv_sec = end.tv_sec - start.tv_sec;
            temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return temp;
    }

    clock::clock() {
        clock_gettime(CLOCK_MONOTONIC_COARSE, &m_start_time);
    }

    double clock::seconds() const {
        timespec now;
        clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
        timespec elapsed = diff(m_start_time, now);
        return elapsed.tv_sec + elapsed.tv_nsec / 1e9;
    }
}
#elif __APPLE__
#include <mach/mach_time.h>
namespace hope::profiler {
    uint64_t diff(uint64_t start, uint64_t end) {
        return end - start;
    }

    clock::clock() {
        m_start_time = mach_absolute_time();
    }

    double clock::seconds() const {
        auto now = mach_absolute_time();
        uint64_t elapsed = diff(m_start_time, now);
        static mach_timebase_info_data_t timebase;
        if (timebase.denom == 0) {
            mach_timebase_info(&timebase);
        }
        double nanoseconds = static_cast<double>(elapsed) * timebase.numer / timebase.denom;
        return nanoseconds / 1e9;
    }
}
#endif
