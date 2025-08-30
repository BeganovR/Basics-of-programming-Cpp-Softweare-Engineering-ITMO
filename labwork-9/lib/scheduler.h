#pragma once
#include <cstddef>
#include <memory>
#include <stack>
#include <vector>

#include "any.h"
#include "apply.h"
#include "tuple.h"

using TaskID = size_t;

class TTaskScheduler {
   private:
    class Base {
       public:
        virtual ~Base() = default;
        virtual TaskID GetID() = 0;
        virtual bool IsExecuted() = 0;
        virtual Any GetResult() = 0;
        virtual void Execute() = 0;
    };

    template <typename T>
    class FutureResult {
       public:
        FutureResult(std::shared_ptr<Base> ptr) : ptr_(ptr) {}

        operator T() const {
            if (!ptr_->IsExecuted()) {
                const_cast<std::shared_ptr<Base>&>(ptr_)->Execute();
            }
            return ptr_->GetResult().template Cast<T>();
        }

       private:
        std::shared_ptr<Base> ptr_;
    };

    template <typename Func, typename... Args>
    class Task : public Base {
       public:
        Task(Func func, TaskID id, Args... args)
            : args_(args...), func_(func), id_(id), is_executed_(false) {}

        TaskID GetID() override { return id_; }
        bool IsExecuted() override { return is_executed_; }
        Any GetResult() override { return result_; }
        void Execute() override {
            result_ = my_apply(func_, args_);
            is_executed_ = true;
        }

       private:
        MyTuple<Args...> args_;
        Func func_;
        TaskID id_;
        bool is_executed_ = false;
        Any result_;
    };

    void TopologicalSort() {
        std::vector<bool> visited(uniq_id_);
        while (!order.empty()) {
            order.pop();
        }

        for (TaskID i = 0; i < uniq_id_; ++i) {
            if (!visited[i]) {
                DFS(i, visited);
            }
        }

        while (!order.empty()) {
            order_top_.push_back(order.top());
            order.pop();
        }
    };

    void DFS(TaskID v, std::vector<bool>& visited) {
        visited[v] = true;
        for (auto u : graph_id_[v]) {
            if (!visited[u]) {
                DFS(u, visited);
            }
        }
        order.push(v);
    };

    std::vector<std::shared_ptr<Base>> tasks_;
    std::vector<std::vector<TaskID>> graph_id_;
    std::stack<TaskID> order;
    std::vector<TaskID> order_top_;
    TaskID uniq_id_ = 0;

   public:
    template <typename Func, typename... Args>
    TaskID add(Func&& func, Args&&... args) {
        auto task = std::make_shared<Task<Func, Args...>>(
            std::forward<Func>(func), uniq_id_++, std::forward<Args>(args)...);
        tasks_.push_back(task);
        graph_id_.push_back(std::vector<TaskID>());
        return uniq_id_ - 1;
    }

    template <typename T>
    FutureResult<T> getFutureResult(TaskID task_id) {
        return FutureResult<T>(tasks_[task_id]);
    }

    template <typename T>
    T getResult(TaskID task_id) {
        auto it = tasks_[task_id];
        if (!it->IsExecuted()) {
            executeAll();
        }
        auto result = it->GetResult().template Cast<T>();
        return result;
    }

    void executeAll() {
        TopologicalSort();
        for (int i = order_top_.size() - 1; i >= 0; --i) {
            if (!tasks_[order_top_[i]]->IsExecuted()) {
                tasks_[order_top_[i]]->Execute();
            }
        }
    }
};