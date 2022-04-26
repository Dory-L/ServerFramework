#pragma once

#include <string.h>
#include <assert.h>
#include "util.h"

#define DORY_ASSERT(x) \
    if (!(x)) { \
        DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << dory::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define DORY_ASSERT2(x, w) \
    if (!(x)) { \
        DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << dory::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
