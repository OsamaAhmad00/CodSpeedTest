#pragma once

#ifdef CODSPEED_ENABLED

#include "measurement.hpp"

namespace Profiling {

class SimulationProfiler {
public:
    ALWAYS_INLINE explicit SimulationProfiler() {
        if (++depth_ == 1) {
            // Only the outermost scope starts the measurement
            measurement_start();
        }
    }

    ALWAYS_INLINE ~SimulationProfiler() {
        if (--depth_ == 0) {
            // Only the outermost scope stops the measurement
            measurement_stop();
        }
    }

private:

    static inline thread_local int depth_ = 0;
};

}

#define SIMULATION_SCOPE_CONCAT_INTERNAL(a, b) a##b
#define SIMULATION_SCOPE_CONCAT(a, b) SIMULATION_SCOPE_CONCAT_INTERNAL(a, b)

#define SIMULATION_SCOPE() \
    ::Profiling::SimulationProfiler SIMULATION_SCOPE_CONCAT(simulation_scope__, __LINE__) { }


#else

// !CODSPEED_ENABLED — everything becomes a NOP

namespace Profiling {

struct SimulationProfiler {
  SimulationProfiler() noexcept {}
};

}

#define SIMULATION_SCOPE()

#endif
