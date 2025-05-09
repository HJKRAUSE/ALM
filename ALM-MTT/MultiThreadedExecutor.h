#pragma once

#include <windows.h>
#include <vector>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include "TaskExecutor.h"

namespace ALM {

    /**
     * @brief Executor that uses the Windows thread pool for concurrent task execution.
     */
    class MultiThreadedExecutor : public TaskExecutor {
    public:
        MultiThreadedExecutor(
            size_t min_threads = 1,
            size_t max_threads = std::thread::hardware_concurrency())
        {
            thread_pool_ = CreateThreadpool(nullptr);

            SetThreadpoolThreadMinimum(thread_pool_, static_cast<DWORD>(min_threads));
            SetThreadpoolThreadMaximum(thread_pool_, static_cast<DWORD>(max_threads));

            InitializeThreadpoolEnvironment(&environment_);
            SetThreadpoolCallbackPool(&environment_, thread_pool_);
        }

        ~MultiThreadedExecutor() {
            DestroyThreadpoolEnvironment(&environment_);
            CloseThreadpool(thread_pool_);
        }

        void submitAndWait(const std::vector<std::function<void()>>& tasks) override {
            if (tasks.empty()) return;

            HANDLE event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            std::atomic<size_t> remaining(tasks.size());

            for (const auto& task : tasks) {
                auto ctx = std::make_unique<TaskContext>(TaskContext{ task, &remaining, event });

                PTP_WORK work = CreateThreadpoolWork(
                    [](PTP_CALLBACK_INSTANCE, void* param, PTP_WORK) {
                        // Take back ownership of the context
                        std::unique_ptr<TaskContext> ctx(reinterpret_cast<TaskContext*>(param));
                        ctx->task();

                        if (--(*ctx->remaining) == 0) {
                            SetEvent(ctx->event);
                        }
                    },
                    ctx.release(),  // Release ownership to the callback
                    &environment_
                );

                SubmitThreadpoolWork(work);
                CloseThreadpoolWork(work);  // Can close immediately after submit
            }

            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }

    private:
        struct TaskContext {
            std::function<void()> task;
            std::atomic<size_t>* remaining;
            HANDLE event;
        };

        PTP_POOL thread_pool_;
        TP_CALLBACK_ENVIRON environment_;
    };

}
