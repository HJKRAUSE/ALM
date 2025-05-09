#pragma once

#include <vector>
#include <functional>

namespace ALM {

    /**
     * @brief Abstract base class for submitting and waiting on a batch of tasks.
     */
    class TaskExecutor {
    public:
        virtual ~TaskExecutor() = default;

        /**
         * @brief Submit a vector of tasks and wait for all to complete.
         */
        virtual void submitAndWait(const std::vector<std::function<void()>>& tasks) = 0;

    protected:
        TaskExecutor() = default;
    };

}
