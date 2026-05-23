#pragma once

#include <cstdint>

#ifdef CODSPEED_WALLTIME
#include "codspeed.h"
#endif

#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #include <x86intrin.h>
#endif

namespace Profiling {
    extern double tsc_frequency_hz;

    inline uint64_t get_timer() {
#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
        unsigned int aux;
        return __rdtscp(&aux);
#else
#error "Compiler or architecture not supported for raw RDTSC."
#endif
    }

    struct Data {
        const char* name = nullptr;
        uint64_t inclusive_ticks = 0;
        uint64_t exclusive_ticks = 0;
#ifdef CODSPEED_WALLTIME
        codspeed::RawWalltimeBenchmark walltime_benchmark;
#endif
    };

    struct State {
        int id;
        uint64_t start_ticks;
        uint64_t children_ticks = 0;
        uint64_t saved_parent_inclusive = 0;
        State* parent = nullptr;
    };

    extern thread_local State* current_state;

    class WalltimeProfiler {
    public:
        WalltimeProfiler(int id, const char* name);
        ~WalltimeProfiler();
    private:
        State local_state { };
    };

    void calibrate_tsc_frequency();
    double ticks_to_ns(uint64_t ticks);
    double ticks_to_us(uint64_t ticks);
    double ticks_to_ms(uint64_t ticks);
    void write_profile_report_json();
    void print_profile_report();
}

#include "WalltimeHelpers.hpp"