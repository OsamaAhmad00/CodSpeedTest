cmake_minimum_required(VERSION 4.2)
include(FetchContent)

project(CodSpeedTest)

set(CMAKE_CXX_STANDARD 23)

# Enable release mode with debug symbols to display useful profiling data
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

FetchContent_Declare(
    google_benchmark
    GIT_REPOSITORY https://github.com/OsamaAhmad00/codspeed-cpp.git # Target the codspeed cpp repository
    SOURCE_SUBDIR google_benchmark # Make sure to target the google_benchmark subdirectory
    GIT_TAG support-multiple-threads # Or chose a specific version or git ref, check the releases page on the repository
)

# Actual Google Benchmark for reference
# FetchContent_Declare(
#     google_benchmark
#     GIT_REPOSITORY https://github.com/google/benchmark.git
#     GIT_TAG main
# )

FetchContent_MakeAvailable(google_benchmark)

add_executable(CodSpeedTest main.cpp WalltimeProfiler.cpp)

# Link your executable against the `benchmark::benchmark`, the `google_benchmark` library
# Note: the first argument must match the first argument of the `add_executable` call
target_link_libraries(CodSpeedTest benchmark::benchmark)