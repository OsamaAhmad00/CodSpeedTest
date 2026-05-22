#include <chrono>
#include <thread>
#include <benchmark/benchmark.h>

#include "CodSpeedProfiler.hpp"
#include "Profiler.hpp"

auto get_vec(size_t n) {
    std::vector vec(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            vec[i][j] = rand();
        }
    }
    return vec;
}

static void BM_Faster_Add(benchmark::State& state) {
    auto n = state.range(0);
    auto vec = get_vec(n);
    for (auto _ : state) {
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                sum += vec[i][j];
            }
        }
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_Slower_Add(benchmark::State& state) {
    auto n = state.range(0);
    auto vec = get_vec(n);
    for (auto _ : state) {
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                sum += vec[j][i];
            }
        }
        benchmark::DoNotOptimize(sum);
    }
}

// static void BM_NO_BENCHMARK(benchmark::State& state) { }
// BENCHMARK(BM_NO_BENCHMARK)->Arg(100);

BENCHMARK(BM_Faster_Add)->Arg(100);
BENCHMARK(BM_Slower_Add)->Arg(100);

static void burn_cycles(int iterations) {
    volatile int i = iterations;
    int result = 0;
    while (i > 0) {
        i = i - 1;
        result += i;
    }
    benchmark::DoNotOptimize(result);
}

static void manual_hooks_recursive_profiler(int i = 10);

static void manual_hooks_recursive_helper(int i) {
    burn_cycles(500000);
    if (i > 0) {
        manual_hooks_recursive_profiler(i - 1);
    }
    burn_cycles(50000);
}

static void manual_hooks_recursive_profiler(int i) {
    auto name = "main.cpp::manual_hooks_recursive_profiler";
    CODSPEED_SCOPE(name);
    NAMED_PROFILE_SCOPE(name);

    burn_cycles(1000000);

    if (i > 0) {
        manual_hooks_recursive_helper(i - 1);
    }

    burn_cycles(100000);
}

int main(int argc, char **argv) {
    // BENCHMARK_MAIN --------------------------------------------------
    char arg0_default[] = "benchmark";
    char *args_default = arg0_default;
    if (!argv) {
        argc = 1;
        argv = &args_default;
    }
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    // -----------------------------------------------------------------

    Profiling::calibrate_tsc_frequency();
    manual_hooks_recursive_profiler();
    Profiling::print_profile_report();
    Profiling::write_profile_report_json();
}