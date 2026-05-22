#pragma once

#ifdef CODSPEED_ENABLED

#include <string>
#include "codspeed.h"
#include "measurement.hpp"

namespace codspeed {

class ScopeProfiler {
public:
    ALWAYS_INLINE explicit ScopeProfiler(const std::string& name) {
        if (++depth_ == 1) {
            // Only the outermost scope starts the measurement
            CodSpeed::getInstance()->start_benchmark(name);
            measurement_start();
        }
    }

    ALWAYS_INLINE ~ScopeProfiler() {
        if (--depth_ == 0) {
            // Only the outermost scope stops the measurement
            measurement_stop();
            CodSpeed::getInstance()->end_benchmark();
        }
    }

private:
    static inline thread_local int depth_ = 0;
};

}

// explicit name (must contain "::" for a valid URI)
#define CODSPEED_SCOPE(name) \
  ::codspeed::ScopeProfiler _codspeed_scope_##__LINE__(name)

// auto-derives the URI from __FILE__ + __func__
#define CODSPEED_SCOPE_AUTO()                                              \
  ::codspeed::ScopeProfiler _codspeed_scope_##__LINE__(                   \
      ::codspeed::get_path_relative_to_workspace(__FILE__) +              \
      "::" + __func__)

#else

// !CODSPEED_ENABLED — everything becomes a NOP

#include <string>

namespace codspeed {

struct ScopeProfiler {
  explicit ScopeProfiler(const char*) noexcept {}
  explicit ScopeProfiler(const std::string&) noexcept {}
};

}

#define CODSPEED_SCOPE(name)    ((void)0)
#define CODSPEED_SCOPE_AUTO()   ((void)0)

#endif
