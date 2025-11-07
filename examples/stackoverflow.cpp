#include <beman/execution/execution.hpp>
#include <coroutine>
#include <iostream>
#include <type_traits>
#include <utility>

namespace ex = beman::execution;

struct task {
    using sender_concept        = ex::sender_t;
    using completion_signatures = ex::completion_signatures<ex::set_value_t()>;

    struct base {
        virtual void complete_value() noexcept = 0;
    };

    struct promise_type {
        struct final_awaiter {
            base* data;
            bool  await_ready() noexcept { return false; }
            auto  await_suspend(auto h) noexcept {
                std::cout << "final_awaiter\n";
                this->data->complete_value();
                std::cout << "completed\n";
            };
            void await_resume() noexcept {}
        };
        std::suspend_always     initial_suspend() const noexcept { return {}; }
        final_awaiter           final_suspend() const noexcept { return {this->data}; }
        void                    unhandled_exception() const noexcept {}
        std::coroutine_handle<> unhandled_stopped() { return std::coroutine_handle<>(); }
        auto                    return_void() {}
        auto get_return_object() { return task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        template <::beman::execution::sender Sender>
        auto await_transform(Sender&& sender) noexcept {
            return ::beman::execution::as_awaitable(::std::forward<Sender>(sender), *this);
        }

        base* data{};
    };

    template <ex::receiver R>
    struct state : base {

        using operation_state_concept = ex::operation_state_t;
        std::remove_cvref_t<R>              r;
        std::coroutine_handle<promise_type> handle;

        state(auto&& r, auto&& h) : r(std::forward<R>(r)), handle(std::move(h)) {}
        void start() & noexcept {
            this->handle.promise().data = this;
            this->handle.resume();
        }
        void complete_value() noexcept override { ex::set_value(std::move(this->r)); }
    };

    std::coroutine_handle<promise_type> handle;

    template <ex::receiver R>
    auto connect(R&& r) && {
        return state<R>(std::forward<R>(r), std::move(this->handle));
    }
};

int main(int ac, char*[]) {
    static_assert(ex::sender<task>);
    ex::sync_wait([](int n) -> task {
        for (int i{}; i < n; ++i) {
            std::cout << "await=" << (co_await ex::just(i)) << "\n";
        }
        co_return;
    }(ac < 2 ? 3 : 30000));
}
