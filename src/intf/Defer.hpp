#pragma once

#include <Common.hpp>

#ifdef __cplusplus

namespace Hyper {

template <typename Func> struct __defer_instance {
    Func m_func;
    __defer_instance(Func &&f) : m_func(f) {}
    ~__defer_instance() { m_func(); }
};

struct __defer_helper {
    template <typename Func> __defer_instance<Func> operator%(Func &&f) {
        return __defer_instance<Func>(static_cast<Func &&>(f));
    }
};

#define defer                                                                  \
    auto CAT(__defer_instance_, __COUNTER__) = Hyper::__defer_helper{} % [&]()

} // namespace Hyper

#endif