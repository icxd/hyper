#pragma once

#include <stdio.h>
#include <sys/cdefs.h>

#include <Common.hpp>

namespace Hyper {

template <typename T> struct Optional {
    Optional(T value) : m_value(value), m_has_value(true) {}
    Optional() : m_has_value(false) {}

    T value() { return m_value; }
    bool has_value() { return m_has_value; }

    T unwrap_or(T default_value) {
        return m_has_value ? m_value : default_value;
    }

    T unwrap() {
        if (!m_has_value)
            PANIC("called `Optional::unwrap()` on a `None` value");
        return m_value;
    }

    operator bool() { return m_has_value; }

  private:
    T m_value;
    bool m_has_value;
};

} // namespace Hyper

using Hyper::Optional;