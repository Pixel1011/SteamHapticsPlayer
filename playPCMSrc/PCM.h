#pragma once

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <TritonController.h>

#ifdef _WIN32
  using path_t = std::wstring;
  #define POPEN wpopen
  #define PCLOSE pclose
  // i hate windows
  #include "ihatewindows.h"
#else
  using path_t = std::string;
  #define POPEN popen
  #define PCLOSE pclose
#endif

class PCM {
private:
  path_t filePath;
  FILE* pipe = nullptr;
  bool ended = false;
  std::vector<uint8_t> pcmBytes = std::vector<uint8_t>();
  size_t readPointer = 0;
  TritonPCMMode audioFormat;
  static constexpr int CHUNK_SIZE = 8;
  
  path_t buildCommand() const;
  void start();
  
  public:
  std::uintmax_t fileSize;
  PCM();
  ~PCM();
  
  int load(const path_t& filePath, TritonPCMMode mode);

  int getNextChunk(uint8_t* buffer);
  // Read up to `size` bytes into buffer. Returns number of bytes read, 0 on EOF, -1 on error
  int getBytes(uint8_t* buffer, int size);

  void reset();
};