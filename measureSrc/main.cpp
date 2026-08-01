#include "ControllerFinder.h"
#include "Utils.h"
#include <Constants.h>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <string>
#include <thread>

TritonController* c = nullptr;
ControllerFinder finder;

int main(int argc, char* argv[]) {
  TritonController* c = finder.getController();
  if (c == nullptr) {
    return 1;
  }
  c->startPoll();

  auto start = std::chrono::steady_clock::now();
  uint64_t initcount = c->stateCounter.load();
  int count = 0;
  int toCount = 5000;

  uint64_t realcount = 0;
  std::string str = std::string("Count: ");

  Utils::ProgressHelper helper(toCount, &str);

  while (count < toCount) {
    if (c->stateCounter.load() - initcount > static_cast<uint64_t>(count)) {
      helper.step();
      count++;
      realcount = c->stateCounter.load() - initcount;
    }
  }
  printf("\n");
  auto elapsed = std::chrono::duration<double>(
                     std::chrono::steady_clock::now() - start)
                     .count();
  printf("0x45 reports: %d in %.2fs = %.1f Hz\n", toCount, elapsed, toCount / elapsed);
  printf("actual count: %lld", realcount);
  c->close();
  return 0;
}