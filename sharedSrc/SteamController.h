#pragma once
#include <hidapi.h>
#include <cstdint>
#include "Constants.h"
enum class ControllerType {
	None,
	Original,
	Triton,
	Jupiter,
	Galileo
};

class SteamController
{
  public:
  ControllerType type = ControllerType::None;

  SteamController(ControllerType type) {
    this->type = type;
  };
  virtual ~SteamController() = default;
  virtual void close() = 0;
  virtual int playNote(int channel, int note, int velocity) = 0;
  virtual int playFrequency(int channel, double frequency, int velocity) = 0;
  virtual int sendRaw(uint8_t bytes[], size_t length) = 0;
  
};