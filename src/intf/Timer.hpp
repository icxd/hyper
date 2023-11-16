#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

struct Timer {
    void initTimer(uint32_t frequency);

  private:
    uint32_t tick = 0;
};

} // namespace Hyper

#ifdef __cplusplus
}
#endif