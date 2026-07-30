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
    //mode = TritonPCMMode::Khz8_16Bit;
    // it was not happy with that
    // idk i guess best audio quality we can do is 8khz 8bit ulaw
    // also i swear to god i have been testing for so long that i am hallucinating hearing birdbrain
    // help
    //
    // also if someone does make it work, remind me to fix my progress code because it doubles with 16 bit :p 
    // also also idk if my thing of not including the last byte has any effect, though i think its ok as there is a length check anyway? does annoy me that 2 bits are being wasted per packet
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
      uint8_t left = tmp[i * 2];
      uint8_t right = tmp[i * 2 + 1];
      packet.left[i] = left;
      packet.right[i] = right;
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