#pragma once
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <format>
#include <chrono>

namespace Utils {
// double defaultMap(double x) { return x; }
std::string formatTime(double totalSeconds);

enum class Mode {
  DEFAULT,
  PROGRESSBAR,
  TIME
};


class ProgressHelper {
private:
  int totalSteps = 0;
  int steps = 1;
  Mode mode;
  int timescale = 1;
  std::string *startString;

  int progress = 0;
  size_t lastStatusLength = 0;

  void defaultString(std::ostringstream* ss, int progress, int totalSteps);
  void defaultStringProgressbar(std::ostringstream* ss, int progress, int totalSteps);
  
  void timeStringProgressbar(std::ostringstream* ss, int progress, int totalSteps);

public:
  // i am going insane, i have absolutely no idea why a segfault is occuring due to the string, i pray using a pointer fixes it;
  ProgressHelper(int totalSteps, std::string *start, int step = 1, Mode mode = Mode::DEFAULT);
  ~ProgressHelper();
  void setTimescale(int timescale);
  int getProgress();
  void step();
};
} // namespace Utils