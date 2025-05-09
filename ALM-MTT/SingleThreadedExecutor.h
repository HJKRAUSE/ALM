#pragma once

#include <vector>
#include <functional>
#include "TaskExecutor.h"

namespace ALM {

    /**
     * @brief Executor that runs all tasks sequentially on the calling thread.
     */
    class SingleThreadedExecutor : public TaskExecutor {
    public:
        void submitAndWait(const std::vector<std::function<void()>>& tasks) override {
            for (const auto& task : tasks) {
                task();
            }
        }
    };

}