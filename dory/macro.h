#pragma once

#include <string.h>
#include <assert.h>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
#   define DORY_LIKELY(x)      __builtin_expect(!!(x), 1)
#   define DORY_UNLIKELY(x)      __builtin_expect(!!(x), 0)
#else
#   define DORY_LIKELY(x)       (x)
#   define DORY_UNLIKELY(x)       (x)
#endif

#define DORY_ASSERT(x) \
    if (DORY_UNLIKELY(!(x))) { \
        DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << dory::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define DORY_ASSERT2(x, w) \
    if (DORY_UNLIKELY(!(x))) { \
        DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << dory::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
