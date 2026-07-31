#include "PCM.h"
#include <ControllerFinder.h>
#include <TritonController.h>
#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <Utils.h>

TritonController* c = nullptr;
ControllerFinder finder;
// if anyone is wondering why this is called aou, consult this video https://www.youtube.com/watch?v=kiFCFlAUy_8
PCM aou;

struct Args {
  bool setup = true;
  path_t filePath;
  std::string help;
};

TritonPCMMode mode = TritonPCMMode::Khz8_8Bit_ulaw;


const std::string helpString =
    "Usage: steam-haptics-singer.exe <file path>";

template <typename ArgGetter>
Args parseArgs(int argc, ArgGetter argAt) {
  Args args;

  for (int i = 1; i < argc; ++i) {
    path_t arg = argAt(i);
    std::string argStr = std::filesystem::path(arg).string();

    if (argStr == "-s") {
      args.setup = false;
    } else if (!argStr.empty() && argStr[0] == '-') {
      args.help = helpString;
      return args;
    } else {
      args.filePath = arg;
    }
  }

  if (args.filePath.empty()) args.help = helpString;
  return args;
}

int runPlayer(const Args& args) {
  if (!args.help.empty()) {
    std::cout << helpString;
    return 1;
  }

  SteamController* cont = finder.getController();
  if (cont == nullptr) return 1;
  if (cont->type == ControllerType::Triton) c = static_cast<TritonController*>(cont);
  if (c == nullptr) return 1;

  if (c->connectionType == TritonInterface::WIRED) {
    // do wonder if itll be happy with that
    mode = TritonPCMMode::Khz8_16Bit;
    // after writing code where i was not sleep deprived, it was happy with it. :D
  }

  
  int loadResult = aou.load(args.filePath, mode);
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

  c->setupPCMStreaming(mode);

  int SAMPLE_RATE = 8000;
  int BYTES_PER_FRAME = 2;
  int SAMPLES_PER_PACKET = 31;
  int bytesPerPacket = 31;

  if (mode == TritonPCMMode::Khz1_16Bit || mode == TritonPCMMode::Khz2_16Bit || mode == TritonPCMMode::Khz4_16Bit || mode == TritonPCMMode::Khz8_16Bit) {
    BYTES_PER_FRAME = 4;
    SAMPLES_PER_PACKET = 15;
    bytesPerPacket = 30;
  } 
  
  int NEED_BYTES = SAMPLES_PER_PACKET * BYTES_PER_FRAME;

  if (mode == TritonPCMMode::Khz1_16Bit || mode == TritonPCMMode::Khz1_8Bit || mode == TritonPCMMode::Khz1_8Bit_ulaw) {
    SAMPLE_RATE = 1000;
  } else
  if (mode == TritonPCMMode::Khz2_16Bit || mode == TritonPCMMode::Khz2_8Bit || mode == TritonPCMMode::Khz2_8Bit_ulaw) {
    SAMPLE_RATE = 2000;
  } else
  if (mode == TritonPCMMode::Khz4_16Bit || mode == TritonPCMMode::Khz4_8Bit || mode == TritonPCMMode::Khz4_8Bit_ulaw) {
    SAMPLE_RATE = 4000;
  } else
  if (mode == TritonPCMMode::Khz8_16Bit || mode == TritonPCMMode::Khz8_8Bit || mode == TritonPCMMode::Khz8_8Bit_ulaw) {
    SAMPLE_RATE = 8000;
  } 
  
  auto period = std::chrono::microseconds((SAMPLES_PER_PACKET * 1000000) / SAMPLE_RATE);

  uint8_t primeBuf[NEED_BYTES];
  int pr = aou.getBytes(primeBuf, NEED_BYTES);
  (void)pr;

  auto nextPacketTime = std::chrono::steady_clock::now();

  // puck has poll rate of 500hz, with 64 bit packets, which = about 256kbit/s, which is technically enough, but with it also feeding data, not enough bandwidth, so 16 bit for wired only
  // wired controller has poll rate of 1000hz, so 512kbit/s
  if (mode == TritonPCMMode::Khz8_16Bit) std::cout << "Using stereo 16bit 8khz audio for wired mode. (256kbps)\n";
  if (mode == TritonPCMMode::Khz8_8Bit_ulaw) std::cout << "Using stereo 8bit 8khz µlaw audio for puck mode. (128kbps)\n";

  std::cout << "Playing audio...\n";

  MsgHapticPCMStereo packet;
  int totalSteps = aou.fileSize - NEED_BYTES;
  std::string start = "Playing: ";
  Utils::ProgressHelper progress(totalSteps, &start, NEED_BYTES, Utils::Mode::TIME);

  while (true) {
    uint8_t tmp[NEED_BYTES];
    int r = aou.getBytes(tmp, NEED_BYTES);
    if (r <= 0) break;
    if (r < NEED_BYTES) std::memset(tmp + r, 0, NEED_BYTES - r); // s8 silence = 0

    packet.length = bytesPerPacket;
 
    for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
      if (BYTES_PER_FRAME == 4) {
        // 60 bytes over 15 samples
        // 16 bit
        size_t base = i*4;
        uint8_t leftLow = tmp[base];
        uint8_t leftHigh = tmp[base + 1];
        uint8_t rightLow = tmp[base + 2];
        uint8_t rightHigh = tmp[base + 3];

        packet.left[i * 2] = leftLow;
        packet.left[i * 2 + 1] = leftHigh;
        packet.right[i * 2] = rightLow;
        packet.right[i * 2 + 1] = rightHigh;
      } else {
        // 8 bit
        // 62 bytes over 31 samples
        uint8_t left = tmp[i * 2];
        uint8_t right = tmp[i * 2 + 1];
        packet.left[i] = left;
        packet.right[i] = right;
      }
    }

    c->sendPCMStereo(&packet);
    progress.step();
    nextPacketTime += period;

    while (std::chrono::steady_clock::now() < nextPacketTime) {}
  }
  
  std::cout << std::endl;

  return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
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