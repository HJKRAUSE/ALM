/*
    MIT License

    Copyright (c) 2025 Harold James Krause

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include <memory>
#include <shared_mutex>

namespace ALM {

    /**
     * @brief Thread-safe, relinkable smart handle to shared, immutable objects.
     *
     * This class wraps a std::shared_ptr<const T> and allows concurrent reads with safe relinking.
     * It is useful for scenarios like referencing yield curves across projections where updates may occur infrequently.
     */
    template <typename T>
    class RelinkableHandle {
    public:
        RelinkableHandle() = default;

        // Construct from an existing shared_ptr
        explicit RelinkableHandle(std::shared_ptr<const T> ptr)
            : ptr_(std::move(ptr)) {
        }

        // Copy constructor (locks shared access)
        RelinkableHandle(const RelinkableHandle& other) {
            std::shared_lock lock(other.mutex_);
            ptr_ = other.ptr_;
        }

        // Copy assignment (thread-safe with lock ordering)
        RelinkableHandle& operator=(const RelinkableHandle& other) {
            if (this != &other) {
                std::unique_lock lhs_lock(mutex_, std::defer_lock);
                std::shared_lock rhs_lock(other.mutex_, std::defer_lock);
                std::lock(lhs_lock, rhs_lock);
                ptr_ = other.ptr_;
            }
            return *this;
        }

        // Dereference access
        const T& operator*() const {
            std::shared_lock lock(mutex_);
            return *ptr_;
        }

        const T* operator->() const {
            std::shared_lock lock(mutex_);
            return ptr_.get();
        }

        // Access the internal shared_ptr
        std::shared_ptr<const T> get() const {
            std::shared_lock lock(mutex_);
            return ptr_;
        }

        // Replace the stored pointer (thread-safe write)
        void reset(std::shared_ptr<const T> new_ptr) {
            std::unique_lock lock(mutex_);
            ptr_ = std::move(new_ptr);
        }

        // Check whether the handle currently points to anything
        bool isEmpty() const {
            std::shared_lock lock(mutex_);
            return !ptr_;
        }

    private:
        std::shared_ptr<const T> ptr_;
        mutable std::shared_mutex mutex_;
    };

}
