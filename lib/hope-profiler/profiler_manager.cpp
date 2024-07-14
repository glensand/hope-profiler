#include "profiler_manager.h"

#include "hope-profiler/third_party/spsc_bounded_queue.h"
#include "hope-profiler/third_party/mpsc_bounded_queue.h"

#include <cassert>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <unordered_map>

#if defined(_WIN32) || defined(_WIN64)
#include "Windows.h"
static auto get_thread_id() noexcept {
    return (unsigned long)GetCurrentThreadId();
}
#else
#include "pthread.h"
static auto get_thread_id() noexcept {
    unsigned long long tid;
    pthread_threadid_np(NULL, &tid);
    return (unsigned long)tid;
}
#endif

namespace hope::profiler {
    
    manager* __glob_manager = nullptr;

    struct thread_register final {
        const char* name;
        unsigned long thread_id;
    };

    namespace statics {
        // TODO:: threadlocal
        mpsc_bounded_queue<block_data> events{ 8 * 1024 };
        mpsc_bounded_queue<thread_register> pending_threads{ 128 };
        std::unordered_map<const char*, unsigned int> registered_block_names; 
        std::thread manager_thread;
        std::atomic_bool running;
        std::ofstream trace_stream;

        template<typename TValue>
        void write(const TValue& v) {
            trace_stream.write((char*)&v, sizeof(v));
        }

        void write(const std::string& v) {
            write((uint64_t)v.size());
            trace_stream.write(v.data(), v.size());
        } 
    };

    enum command : int8_t {
        register_thread = 0,
        register_trace_name = 1,
        tick = 2,
    };

    manager::manager() {
        statics::running.store(true, std::memory_order_release);
        char time_buf[100];
        time_t rawTime = 0;
        time(&rawTime);
        auto* time = localtime(&rawTime);
        strftime(time_buf, 100, "%d_%m_%Y_%H_%M_%S_", time);
        auto timed_file_name = std::string(time_buf) + "_trace.txt";
        statics::trace_stream.open(timed_file_name, std::ios::binary | std::ios::app);
        statics::manager_thread = std::thread([this]{
            store_proc();
        });
    }

    manager::~manager() {
        statics::running.store(false, std::memory_order_release);
        statics::manager_thread.join();
        statics::trace_stream.close();
    }

    void manager::upload_block(block_data &blk) {
        blk.thread_id = get_thread_id();
        const auto enqueued = statics::events.try_enqueue(blk);
        if (!enqueued) {
            volatile int stub = 0;
        }
        assert(enqueued);
    }

    void manager::register_thread(const char *name) {
        thread_register reg { name, get_thread_id() };
        const auto enq = statics::pending_threads.try_enqueue(reg);
        assert(enq);
    }

    void manager::store_proc() {
        while(statics::running.load(std::memory_order_acquire)) {
            {
                thread_register t;
                while (statics::pending_threads.try_dequeue(t)) {
                    statics::write(command::register_thread);
                    statics::write(std::string(t.name));
                    statics::write(t.thread_id);
                }
            }

            block_data blc;
            while(statics::events.try_dequeue(blc)) {
                uint32_t blc_name_index;
                auto it = statics::registered_block_names.find(blc.block_name);
                if (it != std::end(statics::registered_block_names)) {
                    blc_name_index = it->second;
                } else {
                    blc_name_index = statics::registered_block_names.size();
                    statics::registered_block_names.emplace(blc.block_name, blc_name_index);
                    statics::write(command::register_trace_name);
                    statics::write(std::string(blc.block_name));
                    statics::write(blc_name_index);
                }
                statics::write(command::tick);
                statics::write(blc_name_index);
                statics::write(blc.thread_id);
                statics::write(blc.start_sec);
                statics::write(blc.end_sec);
            }

            // todo:: event
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
