#include <cstdio>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <iomanip>
#include <numbers>
#include <string>
#include <thread>
#include <Constants.h>
#include "TritonFinder.h"

// mainly for now to figure out wtf i am doing
// but will eventually use to figure out the range of notes the steam controller can make nicely
// scratch above, if wondering why this is called range, its because of that, but not its just so i can send raw bytes from terminal and i couldnt be bothered to rename :p
// channel maps
/*
0 - left trackpad
1 - right trackpad
3 - both rumbles
4 - right rumble
5 - everything (or maybe both rumbles?) + restart
6 - both trackpads
7 - nothing
*/
TritonController* c = nullptr;
TritonFinder finder;

void reset(int) {
  // just guessing honestly
  for (int x = 0; x <= 8; x++) {
    c->playFrequency(x, -1, 0);
  }
  c->close();
  exit(0);
}

template <typename T>
double map(T value, T in_min, T in_max, T out_min, T out_max) {
    return out_min + (double)(value - in_min) * (out_max - out_min) / (in_max - in_min);
}

int main(int argc, char* argv[]) {
  TritonController* c = finder.getController();
  if (c == nullptr) {
    return 1;
  }

  signal(SIGINT, reset);

  if (argc < 2) {
    std::cerr << "Usage: range <hex bytes separated by spaces>\n";
    std::cerr << "Example: range 83 05 0x80...\n";
    return 1;
  }

  if (argc - 1 > 64) {
    std::cerr << "Error: maximum of 64 bytes allowed\n";
    return 1;
  }

  uint8_t packet[64] = {};
  try {
    for (int i = 1; i < argc; ++i) {
      std::string byteStr = argv[i];
      if (byteStr.rfind("0x", 0) == 0 || byteStr.rfind("0X", 0) == 0) {
        byteStr = byteStr.substr(2);
      }

      int value = std::stoi(byteStr, nullptr, 16);
      if (value < 0 || value > 0xFF) {
        throw std::out_of_range("byte out of range");
      }
      packet[i - 1] = static_cast<uint8_t>(value);
    }
  } catch (...) {
    std::cerr << "Error: invalid hex byte input\n";
    return 1;
  }

  int length = argc - 1;
  std::cout << "Sending " << length << " bytes: ";
  for (int i = 0; i < length; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)packet[i] << ' ';
  }
  std::cout << std::dec << std::endl;

  c->sendRaw(packet, length);
  c->close();
  return 0;
}