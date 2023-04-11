#include "benchmark/benchmark.h"

void
BM_empty(benchmark::State& state)
{
  fprintf(stderr, "benchmark setup");
  for (auto _ : state)
    {
      benchmark::DoNotOptimize(state.iterations());
    }
}
BENCHMARK(BM_empty);
