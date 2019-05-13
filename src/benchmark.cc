// Copyright (C) 2018 Michał Šrajer <michal@post.pl>
// This software is licensed under (one of your choice):
// - MIT
// - Apache-2.0
// - LGPL-2.1
// See LICENSE-*.txt for details.

#include <vector>
#include <random>
//#include <algorithm>
#include "stl_algo.h"
#include <cstdlib>


#include "benchmark/benchmark.h"

size_t SIZE = 1024 << 4;

static void BM_BaseInt(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back(dist(mt));
    }
  }
}

static void BM_SortInt(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back(dist(mt));
    }

    std::sort(v.begin(), v.end());
  }
}

static void BM_StableSortInt(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back(dist(mt));
    }

    std::stable_sort(v.begin(), v.end());
  }
}


static void BM_DtStableSortInt(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<int> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back(dist(mt));
    }

    std::dt_stable_sort(v.begin(), v.end());
  }
}

struct S {
  int key;
  int arr[4];
  char data[10240];
};

static void BM_BaseStruct(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<S> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back({dist(mt)});
    }
  }
}

static void BM_SortStruct(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<S> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back({dist(mt)});
    }

    std::sort(v.begin(), v.end(),
              [](const S &a, const S &b) { return a.key < b.key; });
  }
}

static void BM_StableSortStruct(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<S> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back({dist(mt)});
    }

    std::stable_sort(v.begin(), v.end(),
                     [](const S &a, const S &b) { return a.key < b.key; });
  }
}


static void BM_DtStableSortStruct(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, SIZE);

  while (state.KeepRunning()) {
    std::vector<S> v;
    v.reserve(state.range_x());
    for (int i = 0; i < state.range_x(); i++) {
      v.push_back({dist(mt)});
    }

    std::dt_stable_sort(v.begin(), v.end(),
                     [](const S &a, const S &b) { return a.key < b.key; });
  }
}


//BENCHMARK_MAIN();
  int main(int argc, char** argv) {
    ::benchmark::Initialize(&argc, argv);

    if (getenv("SIZE")) {
        SIZE=std::atol(getenv("SIZE"));        
    }

    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) { 
        return 1;
    }

    BENCHMARK(BM_BaseInt)->Arg(SIZE);
    BENCHMARK(BM_SortInt)->Arg(SIZE);
    BENCHMARK(BM_StableSortInt)->Arg(SIZE);
    BENCHMARK(BM_DtStableSortInt)->Arg(SIZE);
    BENCHMARK(BM_BaseStruct)->Arg(SIZE);
    BENCHMARK(BM_SortStruct)->Arg(SIZE);
    BENCHMARK(BM_StableSortStruct)->Arg(SIZE);
    BENCHMARK(BM_DtStableSortStruct)->Arg(SIZE);
    
    ::benchmark::RunSpecifiedBenchmarks();
  }
