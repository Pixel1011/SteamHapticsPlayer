.PHONY: all release
all: range steam-haptics-player measure
release: all


CXXFLAGS = -std=c++20 -Wall -Werror

DEBUG_FLAGS = -g -Og
RELEASE_FLAGS = -Os

ifeq ($(filter release,$(MAKECMDGOALS)), release)
  CXXFLAGS += $(RELEASE_FLAGS)
else
  CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(OS),Windows_NT)
HIDAPI_PKG ?= hidapi
UNICODE_FLAG ?= -municode
CXXFLAGS += -static
else
HIDAPI_PKG ?= hidapi-hidraw
CXXFLAGS += -fPIC
LDFLAGS += -pie
endif

CXXFLAGS += `pkg-config --libs --cflags $(HIDAPI_PKG)`


SHARED_SRC = $(wildcard sharedSrc/*.cpp sharedSrc/*/*.cpp sharedSrc/*.c sharedSrc/*/*.c)
TRITON_SRC := $(wildcard TritonLib/src/*.cpp TritonLib/src/*/*.cpp TritonLib/src/*.c TritonLib/src/*/*.c)
RANGE_SRC = $(wildcard rangeSrc/*.cpp rangeSrc/*/*.cpp rangeSrc/*.c rangeSrc/*/*.c)
PCM_SRC = $(wildcard playPCMSrc/*.cpp playPCMSrc/*/*.cpp playPCMSrc/*.c playPCMSrc/*/*.c)
MEASURE_SRC = $(wildcard measureSrc/*.cpp measureSrc/*/*.cpp measureSrc/*.c measureSrc/*/*.c)

range: $(RANGE_SRC) $(SHARED_SRC) $(TRITON_SRC)
	g++ -IsharedSrc -ITritonLib/include -o range $^ $(CXXFLAGS) $(LDFLAGS)

steam-haptics-player: ${PCM_SRC} ${SHARED_SRC} $(TRITON_SRC)
	g++ -IsharedSrc -ITritonLib/include $(UNICODE_FLAG) -o steam-haptics-player $^ $(CXXFLAGS) $(LDFLAGS)

measure: ${MEASURE_SRC} ${SHARED_SRC} $(TRITON_SRC)
	g++ -IsharedSrc -ITritonLib/include -o measure $^ $(CXXFLAGS) $(LDFLAGS)