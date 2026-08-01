#include "ControllerFinder.h"
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

  auto start = std::chrono::steady_clock::now();
  int count = 0;
  while (count < 5000) {
    //unsigned char buf[64];
    ///int n = hid_read(c->hid_handle, buf, sizeof(buf));
    //if (n > 0 && buf[0] == 0x45) count++;
  }
  auto elapsed = std::chrono::duration<double>(
                     std::chrono::steady_clock::now() - start)
                     .count();
  printf("0x45 reports: %d in %.2fs = %.1f Hz\n", count, elapsed, count / elapsed);

  c->close();
  return 0;
}