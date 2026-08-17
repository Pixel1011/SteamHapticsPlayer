
ifeq ($(OS),Windows_NT)
HIDAPI_PKG ?= hidapi
UNICODE_FLAG ?= -municode
LDFLAGS += -static
BUILD_DIR := build/win
EXT := .exe
else
HIDAPI_PKG ?= hidapi-hidraw
CXXFLAGS += -fPIC
LDFLAGS += -pie
BUILD_DIR := build/linux
EXT :=
endif

.PHONY: all release clean FORCE
all: range$(EXT) steam-haptics-player$(EXT) measure$(EXT)
release: all

CXXFLAGS = -std=c++20 -Wall -Werror
DEBUG_FLAGS = -g -Og
# -Os results in linker error regarding std::basic_string ???? but -O2 compiles and works fine???? - only on windows msy2 tho
RELEASE_FLAGS = -O2

ifeq ($(filter release,$(MAKECMDGOALS)), release)
  CXXFLAGS += $(RELEASE_FLAGS)
else
  CXXFLAGS += $(DEBUG_FLAGS)
endif

CXXFLAGS += $(shell pkg-config --cflags $(HIDAPI_PKG))
CXXFLAGS +=-IsharedSrc -ITritonLib/include
LDLIBS += $(shell pkg-config --libs $(HIDAPI_PKG))

SHARED_SRC = $(wildcard sharedSrc/*.cpp sharedSrc/*/*.cpp sharedSrc/*.c sharedSrc/*/*.c)
TRITON_SRC := $(wildcard TritonLib/src/*.cpp TritonLib/src/*/*.cpp TritonLib/src/*.c TritonLib/src/*/*.c)
RANGE_SRC = $(wildcard rangeSrc/*.cpp rangeSrc/*/*.cpp rangeSrc/*.c rangeSrc/*/*.c)
PCM_SRC = $(wildcard playPCMSrc/*.cpp playPCMSrc/*/*.cpp playPCMSrc/*.c playPCMSrc/*/*.c)
MEASURE_SRC = $(wildcard measureSrc/*.cpp measureSrc/*/*.cpp measureSrc/*.c measureSrc/*/*.c)

objects = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(1))) $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(1)))
SHARED_OBJ := $(call objects,$(SHARED_SRC))
TRITON_OBJ := $(call objects,$(TRITON_SRC))
RANGE_OBJ  := $(call objects,$(RANGE_SRC))
PCM_OBJ    := $(call objects,$(PCM_SRC))
MEASURE_OBJ := $(call objects,$(MEASURE_SRC))

ALL_OBJ := $(SHARED_OBJ) $(TRITON_OBJ) $(RANGE_OBJ) $(PCM_OBJ) $(MEASURE_OBJ)
DEPENDENCY_FILES := $(ALL_OBJ:.o=.d)
FLAGS_FILE := $(BUILD_DIR)/.compiler-flags

FORCE:
$(FLAGS_FILE): FORCE
	@mkdir -p $(dir $@)
	@printf '%s\n' '$(CXXFLAGS)' | cmp -s - $@ || printf '%s\n' '$(CXXFLAGS)' > $@


range$(EXT): $(RANGE_OBJ) $(SHARED_OBJ) $(TRITON_OBJ)
	g++ $(LDFLAGS) -o $@ $(RANGE_OBJ) $(SHARED_OBJ) $(TRITON_OBJ) $(LDLIBS) -lwinmm
ifeq ($(filter release,$(MAKECMDGOALS)), release)
	strip $@
endif

steam-haptics-player$(EXT): $(PCM_OBJ) $(SHARED_OBJ) $(TRITON_OBJ)
	g++ $(LDFLAGS) $(UNICODE_FLAG) -o $@ $(PCM_OBJ) $(SHARED_OBJ) $(TRITON_OBJ) $(LDLIBS)
ifeq ($(filter release,$(MAKECMDGOALS)), release)
	strip $@
endif

measure$(EXT) : $(MEASURE_OBJ) $(SHARED_OBJ) $(TRITON_OBJ)
	g++ $(LDFLAGS) -o $@ $(MEASURE_OBJ) $(SHARED_OBJ) $(TRITON_OBJ) $(LDLIBS)
ifeq ($(filter release,$(MAKECMDGOALS)), release)
	strip $@
endif

$(BUILD_DIR)/%.o: %.cpp $(FLAGS_FILE)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDENCY_FILES)

clean:
	rm -rf $(BUILD_DIR) range steam-haptics-player measure