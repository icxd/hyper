#pragma once

#ifdef __cplusplus
extern "C" {
#endif

namespace Hyper {

#define CAT_HELPER(x, y) x##y
#define CAT(x, y) CAT_HELPER(x, y)

} // namespace Hyper

#ifdef __cplusplus
}
#endif