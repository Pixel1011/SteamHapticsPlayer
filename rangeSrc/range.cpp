#include "TritonFinder.h"
#include <chrono>
#ifdef __WIN32
#include <windows.h>
#include <timeapi.h>
#endif
#include <sstream>
#include <bitset>
TritonController* c = nullptr;
TritonFinder finder;

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
}*/

#define PRINT(obj, field) out << #field << ": " << +(obj).field << "\x1b[K\n"
#define PRINTBIN(obj, field) out << #field << ": " << std::bitset<32>((obj).field) << "\x1b[K\n"

int main(int argc, char* argv[]) {
  TritonController* c = finder.getController();
  if (c == nullptr) {
    return 1;
  }
  #ifdef __WIN32
    timeBeginPeriod(0);
  #endif
  
  /*FeatureReportMsg msg{};
  msg.header.type = ID_SET_SETTINGS_VALUES;
  msg.header.length = sizeof(ControllerSetting);
  msg.payload.setSettingsValues.settings[0].settingNum = SETTING_IMU_MODE;
  msg.payload.setSettingsValues.settings[0].settingValue = 0x00;
  c->sendFeatureReport(&msg, sizeof(msg));
  
 std::this_thread::sleep_for(std::chrono::milliseconds(50));
   FeatureReportMsg msg2{};
  msg2.header.type = ID_SET_SETTINGS_VALUES;
  msg2.header.length = sizeof(ControllerSetting);
  msg2.payload.setSettingsValues.settings[0].settingNum = SETTING_IMU_MODE;
  msg2.payload.setSettingsValues.settings[0].settingValue = 0xffff;
  c->sendFeatureReport(&msg2, sizeof(msg2));*/
  
  c->startPoll(true);
  c->setLizardMode(LIZARD_MODE_ON);
  const int hz = 260;
  auto nextTime = std::chrono::steady_clock::now();

  while (1) {

    if (!c || c->disconnected) {
      if (c!= nullptr && c->disconnected) delete c;
      c = finder.getController();
      if (!c) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        continue;
      }
      c->startPoll(true);
    }

    nextTime += std::chrono::microseconds(1000000/hz);
    auto updates = c->pollUpdates();
    for (TritonInputUpdate update : updates) {
      std::ostringstream out;
      out << "\x1b[H";
      PRINTBIN(update.state, buttons);
      PRINTBIN(update, pressed);
      PRINTBIN(update, released);
      PRINT(update.state, imu.sAccelX);
      PRINT(update.state, imu.sAccelY);
      PRINT(update.state, imu.sAccelZ);
      PRINT(update.state, imu.sGyroX);
      PRINT(update.state, imu.sGyroY);
      PRINT(update.state, imu.sGyroZ);
      PRINT(update.state, imu.sGyroQuatW);
      PRINT(update.state, imu.sGyroQuatX);
      PRINT(update.state, imu.sGyroQuatY);
      PRINT(update.state, imu.sGyroQuatZ);
      PRINT(update.state, seq_num);
      PRINT(update.state, sTriggerLeft);
      PRINT(update.state, sTriggerRight);
      PRINT(update.state, sLeftStickX);
      PRINT(update.state, sLeftStickY);
      PRINT(update.state, sRightStickX);
      PRINT(update.state, sRightStickY);
      PRINT(update.state, sLeftPadX);
      PRINT(update.state, sLeftPadY);
      PRINT(update.state, unPressureLeft);
      PRINT(update.state, sRightPadX);
      PRINT(update.state, sRightPadY);
      PRINT(update.state, unPressureRight);
      out << "\x1b[J";
      std::cout << out.str() << std::flush;
    }
    std::this_thread::sleep_until(nextTime);
  }
  return 0;

}
