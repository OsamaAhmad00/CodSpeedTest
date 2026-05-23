#pragma once

#if defined(PROFILE_WALLTIME) || defined(CODSPEED_WALLTIME)

#include <cstddef>

#define WALLTIME_CONCAT_HELPER(x, y) x##y
#define WALLTIME_CONCAT(x, y) WALLTIME_CONCAT_HELPER(x, y)

#define WALLTIME_STRINGIFY_HELPER(x) #x
#define WALLTIME_STRINGIFY(x) WALLTIME_STRINGIFY_HELPER(x)

namespace Profiling::detail {

template <std::size_t N>
struct fixed_string {
    char data[N]{};
    static constexpr std::size_t size = N - 1;

    constexpr fixed_string(const char (&str)[N]) {
        for (std::size_t i = 0; i < N; ++i) data[i] = str[i];
    }

    constexpr fixed_string() = default;
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

template <fixed_string File, fixed_string Func, fixed_string Line>
constexpr auto create_constexpr_name() {
    constexpr std::size_t total_size = File.size + 2 + Func.size + 1 + Line.size;
    fixed_string<total_size + 1> out{};
    std::size_t idx = 0;

    for (std::size_t i = 0; i < File.size; ++i) out.data[idx++] = File.data[i];
    out.data[idx++] = ':'; out.data[idx++] = ':';
    for (std::size_t i = 0; i < Func.size; ++i) out.data[idx++] = Func.data[i];
    out.data[idx++] = ':';
    for (std::size_t i = 0; i < Line.size; ++i) out.data[idx++] = Line.data[i];
    out.data[idx] = '\0';

    return out;
}

// A template provider class that hosts the final string directly in its type signature
template <fixed_string Str>
struct compile_time_storage {
    static constexpr const char* get() { return Str.data; }
};

}

#define WALLTIME_SCOPE_INSTANTIATE(id, line, name_ptr) \
    Profiling::WalltimeProfiler WALLTIME_CONCAT(walltime_obj__, line)( \
        id, \
        name_ptr \
    )

/*
 * Using __COUNTER__ is not the best idea, since other entities will use it as well
 *  and we'll have some holds in the array. Also, it's per-translation unit, not global.
 * Not using it however means we need to manually detect when two objects are profiling
 *  the same spot, possibly by comparing a constexpr hash of filename + line number
 */

#define WALLTIME_SCOPE() \
    WALLTIME_SCOPE_INSTANTIATE( \
        __COUNTER__, \
        __LINE__, \
        (Profiling::detail::compile_time_storage< \
            Profiling::detail::create_constexpr_name<\
                __FILE__, \
                __FUNCTION__,\
                WALLTIME_STRINGIFY(__LINE__)\
            >() \
         >::get()) \
    )

#define NAMED_WALLTIME_SCOPE(name) \
    WALLTIME_SCOPE_INSTANTIATE(__COUNTER__, __LINE__, name)

#else

#define WALLTIME_SCOPE()
#define NAMED_WALLTIME_SCOPE(name)

#endif