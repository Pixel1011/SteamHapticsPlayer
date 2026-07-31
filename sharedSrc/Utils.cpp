#include "Utils.h"
namespace Utils {

std::string formatTime(double totalSeconds) {
  auto d = std::chrono::duration<double>(totalSeconds);
  auto hms = std::chrono::hh_mm_ss{d};
  auto hours = hms.hours().count();
  auto minutes = hms.minutes().count();
  auto seconds = hms.seconds().count();

  if (hours > 0) return std::format("{}:{:02}:{:02}", hours, minutes, seconds);
  return std::format("{:02}:{:02}", minutes, seconds);
}

void ProgressHelper::defaultString(std::ostringstream* ss, int progress, int totalSteps) {
  double percent = (100.0 * progress) / (double)totalSteps;
  std::string text = std::format("{}{}/{} ({:.1f}%)", *startString, progress, totalSteps, percent);
  *ss << text;
}

void ProgressHelper::defaultStringProgressbar(std::ostringstream* ss, int progress, int totalSteps) {
  if (totalSteps <= 0) return;
  double percent = (100.0 * progress) / (double)totalSteps;
  char barChar = '=';
  int totalChars = 50;

  int numToAdd = static_cast<int>((progress / static_cast<double>(totalSteps)) * totalChars);
  std::string filled = std::string(numToAdd, barChar);
  if (numToAdd > 0) filled[numToAdd - 1] = '>';

  std::string empty = std::string(totalChars - numToAdd, ' ');
  std::string bar = filled + empty;

  std::string text = std::format("{}{}/{} [{}] ({:.1f}%)", *startString, progress, totalSteps, bar, percent);
  *ss << text;
}

void ProgressHelper::timeStringProgressbar(std::ostringstream* ss, int progress, int totalSteps) {
  if (totalSteps <= 0) return;
  double percent = (100.0 * progress) / (double)totalSteps;
  char barChar = '=';
  int totalChars = 50;

  int numToAdd = static_cast<int>((progress / static_cast<double>(totalSteps)) * totalChars);

  // i like to make general funcs but oh well this is taking too long
  // stereo (2) 8khz (8000)
  std::string totalTime = formatTime(totalSteps / 2.0 / 8000 / (double)this->timescale);
  std::string timeProgress = formatTime(progress / 2.0 / 8000 / (double)this->timescale);

  std::string filled = std::string(numToAdd, barChar);
  if (numToAdd > 0) filled[numToAdd - 1] = '>';

  std::string empty = std::string(totalChars - numToAdd, ' ');
  std::string bar = filled + empty;

  std::string text = std::format("{}{}/{} [{}] ({:.1f}%)", *startString, timeProgress, totalTime, bar, percent);
  *ss << text;
}

ProgressHelper::ProgressHelper(int totalSteps, std::string* start, int step, Mode mode) {
  this->totalSteps = totalSteps;
  this->steps = step;
  this->mode = mode;
  this->startString = start;
}

ProgressHelper::~ProgressHelper() {
}

void ProgressHelper::setTimescale(int timescale) {
  this->timescale = timescale;
}

void ProgressHelper::step() {
  progress += steps;
  if (progress > totalSteps) progress = totalSteps;

  std::ostringstream ss;
  // getString(&ss, progress, totalSteps);

  switch (mode) {
    case Mode::PROGRESSBAR:
      defaultStringProgressbar(&ss, progress, totalSteps);
      break;
    case Mode::TIME:
      timeStringProgressbar(&ss, progress, totalSteps);
      break;
    default:
      defaultString(&ss, progress, totalSteps);
      break;
  }

  std::string status = ss.str();
  std::cout << '\r' << status;

  if (lastStatusLength > status.size()) std::cout << std::string(lastStatusLength - status.size(), ' ');

  std::cout << std::flush;
  lastStatusLength = status.size();
}

} // namespace Utils