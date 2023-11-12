#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

uint8_t portByteIn(uint16_t port);
uint16_t portWordIn(uint16_t port);
uint32_t portDwordIn(uint16_t port);

void portByteOut(uint16_t port, uint8_t data);
void portWordOut(uint16_t port, uint16_t data);
void portDwordOut(uint16_t port, uint32_t data);

} // namespace Hyper

#ifdef __cplusplus
}
#endif