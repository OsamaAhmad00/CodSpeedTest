#include "Profiler.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace Profiling {
    namespace {
        constexpr size_t profiler_array_max = 1 << 14;
        Data profiler_array[profiler_array_max];
        size_t active_profiler_count = 0;
    }

    double tsc_frequency_hz = 0.0;

    inline thread_local State* current_state = nullptr;

    Profiler::Profiler(int id, std::string name) {
        local_state.id = id;
        local_state.children_ticks = 0;
        local_state.parent = current_state;

        auto& registry_slot = profiler_array[id];
        if (registry_slot.name.empty()) {
            registry_slot.name = std::move(name);
            if (static_cast<size_t>(id) >= active_profiler_count) {
                active_profiler_count = id + 1;
            }
        }

        local_state.saved_parent_inclusive = registry_slot.inclusive_ticks;

        current_state = &local_state;
        local_state.start_ticks = get_timer();
    }

    Profiler::~Profiler() {
        uint64_t end_ticks = get_timer();
        uint64_t total_elapsed = end_ticks - local_state.start_ticks;
        uint64_t exclusive = total_elapsed - local_state.children_ticks;

        auto& registry_slot = profiler_array[local_state.id];

        registry_slot.inclusive_ticks = local_state.saved_parent_inclusive + total_elapsed;
        registry_slot.exclusive_ticks += exclusive;

        if (local_state.parent) {
            local_state.parent->children_ticks += total_elapsed;
        }

        current_state = local_state.parent;
    }

    void calibrate_tsc_frequency() {
        using namespace std::chrono;
        auto start_system = steady_clock::now();
        uint64_t start_tsc = get_timer();

        std::this_thread::sleep_for(milliseconds(50));

        auto end_system = steady_clock::now();
        uint64_t end_tsc = get_timer();

        auto system_duration_ns = duration_cast<nanoseconds>(end_system - start_system).count();
        uint64_t tsc_ticks = end_tsc - start_tsc;

        tsc_frequency_hz = (static_cast<double>(tsc_ticks) / static_cast<double>(system_duration_ns)) * 1000000000.0;
    }

    double ticks_to_ms(const uint64_t ticks) {
        return (static_cast<double>(ticks) / tsc_frequency_hz) * 1000;
    }

    void print_profile_report() {
        std::cout << "\n========================================= PROFILER PERFORMANCE REPORT =========================================\n";
        std::printf("%-75s | %-18s | %-18s\n", "Name", "Inc Time (ms)", "Exc Time (ms)");
        std::cout << "-------------------------------------------------------------------------------------------------------------------------------\n";

        for (size_t i = 0; i < active_profiler_count; ++i) {
            if (!profiler_array[i].name.empty()) {
                const double inc_ms = ticks_to_ms(profiler_array[i].inclusive_ticks);
                const double exc_ms = ticks_to_ms(profiler_array[i].exclusive_ticks);

                std::printf("%-75s | %-18.4f | %-18.4f\n",
                            profiler_array[i].name.c_str(),
                            inc_ms,
                            exc_ms);
            }
        }
        std::cout << "===============================================================================================================================\n";
    }
}