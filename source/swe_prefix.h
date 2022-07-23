#pragma once

///[OLC_HM] START STD_INCLUDES
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
#include <list>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <functional>
#include <string>
#include <algorithm>
#include <fstream>
///[OLC_HM] END STD_INCLUDES

///[OLC_HM] START COMPILER_SWITCHES
// Compiler/System Sensitivity
#if !defined(SOUNDWAVE_USING_WINMM) && !defined(SOUNDWAVE_USING_WASAPI) &&  \
    !defined(SOUNDWAVE_USING_XAUDIO) && !defined(SOUNDWAVE_USING_OPENAL) && \
    !defined(SOUNDWAVE_USING_ALSA) && !defined(SOUNDWAVE_USING_SDLMIXER) && \
    !defined(SOUNDWAVE_USING_PULSE)                                         \

	#if defined(_WIN32)
		#define SOUNDWAVE_USING_WINMM
	#endif
	#if defined(__linux__)
		#define SOUNDWAVE_USING_ALSA
	#endif
	#if defined(__APPLE__)
		#define SOUNDWAVE_USING_SDLMIXER
	#endif
	#if defined(__EMSCRIPTEN__)
		#define SOUNDWAVE_USING_SDLMIXER
	#endif

#endif
///[OLC_HM] END COMPILER_SWITCHES
