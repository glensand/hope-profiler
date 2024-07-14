#include "hope-profiler/profiler.h"

#include <array>
#include <thread>

void mock_function(int recursion) {
    HOPE_SCOPE(RecursiveFunction);
    HOPE_SCOPE(RecursiveFunction2);
    --recursion;
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    if (recursion > 0) {
        mock_function(recursion);
    }
}

int main() {
    hope::profiler::__glob_manager = new hope::profiler::manager;
    HOPE_REG_THREAD(MainThread);
    HOPE_SCOPE(MainScope);
    std::array<std::thread, 50> threads;
    for (int i = 0; i < 50; ++i) {
        HOPE_SCOPE(CreateThread);
        threads[i] = std::thread([]{
            HOPE_REG_THREAD(MockThread);
            mock_function(10);   
        });
    }

    for (auto&& t : threads) {
        HOPE_SCOPE(WaitThread);
        t.join();
    }

    delete hope::profiler::__glob_manager;
    return 0;
}