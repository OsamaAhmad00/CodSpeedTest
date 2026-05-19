#include <benchmark/benchmark.h>

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

static void BM_NO_BENCHMARK(benchmark::State& state) {

}

BENCHMARK(BM_Faster_Add)->Arg(100);
BENCHMARK(BM_NO_BENCHMARK)->Arg(100);
BENCHMARK(BM_Slower_Add)->Arg(100);

BENCHMARK_MAIN();