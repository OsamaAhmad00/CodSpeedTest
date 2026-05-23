#pragma once

#include <string>
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
        std::string name;
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

    class Profiler {
    public:
        Profiler(int id, std::string name);
        ~Profiler();
    private:
        State local_state;
    };

    void calibrate_tsc_frequency();
    double ticks_to_ns(uint64_t ticks);
    double ticks_to_us(uint64_t ticks);
    double ticks_to_ms(uint64_t ticks);
    void write_profile_report_json();
    void print_profile_report();
}

/*
 * Using __COUNTER__ is not the best idea, since other entities will use it as well
 *  and we'll have some holds in the array. Also, it's per-translation unit, not global.
 * Not using it however means we need to manually detect when two objects are profiling
 *  the same spot, possibly by comparing a constexpr hash of filename + line number
 */

#define PROFILER_STRINGIFY_INTERNAL(x) #x
#define PROFILER_CONCAT_INTERNAL(x, y) x##y

#define PROFILER_STRINGIFY(x) PROFILER_STRINGIFY_INTERNAL(x)
#define PROFILER_CONCAT(x, y) PROFILER_CONCAT_INTERNAL(x, y)

#define PROFILE_SCOPE_GENERATED_NAME(id_val) \
    (std::string(__FILE__) + "::[ID:" + PROFILER_STRINGIFY(id_val) + "][Name:" + __FUNCTION__ + "][Line:" + PROFILER_STRINGIFY(__LINE__) + "]")

#define PROFILE_SCOPE_DEVELOP_INTERNAL(id, line) \
    static const int PROFILER_CONCAT(profiler_id_, line) = id; \
    Profiling::Profiler PROFILER_CONCAT(profiler_obj_, line)( \
        PROFILER_CONCAT(profiler_id_, line), \
        PROFILE_SCOPE_GENERATED_NAME(id) \
    );

#define PROFILE_SCOPE() PROFILE_SCOPE_DEVELOP_INTERNAL(__COUNTER__, __LINE__)

#define NAMED_PROFILE_SCOPE(name) \
    Profiling::Profiler PROFILER_CONCAT(profiler_obj_, __LINE__)( \
        __COUNTER__, \
        name \
    );

