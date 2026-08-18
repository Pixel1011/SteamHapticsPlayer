#include "PCM.h"
#include <TritonController.h>
#include <TritonFinder.h>
#include <Utils.h>
#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

TritonController* c = nullptr;
TritonFinder finder;
// if anyone is wondering why this is called aou, consult this video https://www.youtube.com/watch?v=kiFCFlAUy_8
PCM aou;

struct Args {
  bool loop = false;
  path_t filePath;
  std::string help;
  bool ffmpegArgsEnabled = false;
  std::string ffmpegArgs;
};

TritonPCMMode mode = TritonPCMMode::Khz8_8Bit_ulaw;

#ifdef _WIN32
const std::string helpString = "Usage: .\\steam-haptics-player.exe [-l|--loop] [--ffmpegArgs \"<args>\"] <file path>\n";
#else
const std::string helpString = "Usage: ./steam-haptics-player [-l|--loop] [--ffmpegArgs \"<args>\"] <file path>\n";
#endif

Args parseArgs(int argc, std::function<path_t(int index)> argAt) {
  Args args;

  for (int i = 1; i < argc; ++i) {
    path_t arg = argAt(i);
    std::string argStr = std::filesystem::path(arg).string();

    if (argStr == "-l" || argStr == "--loop") {
      args.loop = true;
    } else if (argStr == "--ffmpegArgs") {
      args.ffmpegArgsEnabled = true;
      i += 1;
      if (i >= argc) continue;

      // sure is fun that converting between string and wstring was like that
      std::string ffmpegArgs = std::filesystem::path(argAt(i)).string();
      args.ffmpegArgs = ffmpegArgs;
    } else if (!argStr.empty() && argStr[0] == '-') {
      args.help = helpString;
      return args;
    } else {
      args.filePath = arg;
    }
  }

  if (args.filePath.empty() || (args.ffmpegArgsEnabled && args.ffmpegArgs.empty())) args.help = helpString;
  return args;
}

int runPlayer(const Args& args) {
  if (!args.help.empty()) {
    std::cout << helpString;
    return 1;
  }

  TritonController* c = finder.getController();
  if (c == nullptr) {
    return 1;
  }

  if (c->pairType == ETritonPairType::k_ETritonPairType_Wired) {
    // do wonder if itll be happy with that
    mode = TritonPCMMode::Khz8_16Bit;
    // after writing code where i was not sleep deprived, it was happy with it. :D
  }

  int loadResult = aou.load(args.filePath, mode, args.ffmpegArgs);
  if (loadResult < 0) {
    if (loadResult == -2) {
      std::cout << "ffmpeg was not found on PATH\n";
    } else {
#ifdef _WIN32
      std::wcout << L"ffmpeg could not load " << args.filePath << L"\n";
#else
      std::cout << "ffmpeg could not load " << args.filePath << "\n";
#endif
    }
    return 1;
  }

  if (mode == TritonPCMMode::Khz8_16Bit) std::cout << "Using stereo 16bit 8khz audio for wired mode. (256kbps)\n";
  if (mode == TritonPCMMode::Khz8_8Bit_ulaw) std::cout << "Using stereo 8bit 8khz µlaw audio for puck mode. (128kbps)\n";

  std::cout << "Playing audio...\n";

  std::string start = "Playing: ";
  std::optional<Utils::ProgressHelper> progress;
  const uint8_t* data = aou.pcmBytes.data();

  int timescale = 1;
  if (mode == TritonPCMMode::Khz8_16Bit) timescale = 2;
  do {
    c->playStereoAudio(const_cast<uint8_t*>(data), aou.fileSize, mode, [&](int step, int* readPointer) {
      if (!progress) {
        progress.emplace(aou.fileSize, &start, step, Utils::Mode::TIME);
        progress->setTimescale(timescale);
      }
      progress->step();
    });
    if (c->playThread.joinable()) c->playThread.join();
    progress.reset();
  } while (args.loop);

  std::cout << std::endl;

  return 0;
}

#ifdef _WIN32
#include <windows.h>

int wmain(int argc, wchar_t* argv[]) {
  SetConsoleOutputCP(CP_UTF8);
  Args args = parseArgs(argc, [&](int index) -> path_t {
    return std::wstring(argv[index]);
  });
  return runPlayer(args);
}
#else
int main(int argc, char* argv[]) {
  Args args = parseArgs(argc, [&](int index) -> path_t {
    return std::filesystem::path(argv[index]).string();
  });
  return runPlayer(args);
}
#endif