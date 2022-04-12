#pragma once
#include <thread>
#include <stdint.h>

namespace dory {
pid_t GetThreadId();
uint32_t GetFiberId();

} // namespace dory
