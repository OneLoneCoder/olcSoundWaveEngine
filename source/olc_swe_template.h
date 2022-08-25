/*
	+-------------------------------------------------------------+
	|             OneLoneCoder Sound Wave Engine v0.02            |
	|  "You wanted noise? Well is this loud enough?" - javidx9    |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
	olc::SoundWaveEngine is a single file, cross platform audio
	interface for lightweight applications that just need a bit of
	easy audio manipulation.

	It's origins started in the olcNoiseMaker file that accompanied
	javidx9's "Code-It-Yourself: Synthesizer" series. It was refactored
	and absorbed into the "olcConsoleGameEngine.h" file, and then
	refactored again into olcPGEX_Sound.h, that was an extension to
	the awesome "olcPixelGameEngine.h" file.

	Alas, it went underused and began to rot, with many myths circulating
	that "it doesnt work" and "it shouldn't be used". These untruths
	made javidx9 feel sorry for the poor file, and he decided to breathe
	some new life into it, in anticipation of new videos!

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2022 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met :

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditionsand the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice.This list of conditions and the following
	disclaimer must be reproduced in the documentation and /or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9

	Thanks
	~~~~~~
	Gorbit99, Dragoneye, Puol

	Authors
	~~~~~~~
	slavka, MaGetzUb, cstd, Moros1138 & javidx9

	(c)OneLoneCoder 2019, 2020, 2021, 2022
*/


/*
	Using & Installing On Microsoft Windows
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Microsoft Visual Studio
	~~~~~~~~~~~~~~~~~~~~~~~
	1) Include the header file "olcSoundWaveEngine.h" from a .cpp file in your project.
	2) That's it!


	Code::Blocks
	~~~~~~~~~~~~
	1) Make sure your compiler toolchain is NOT the default one installed with Code::Blocks. That
		one is old, out of date, and a general mess. Instead, use MSYS2 to install a recent and
		decent GCC toolchain, then configure Code::Blocks to use it

		Guide for installing recent GCC for Windows:
		https://www.msys2.org/
		Guide for configuring code::blocks:
		https://solarianprogrammer.com/2019/11/05/install-gcc-windows/
		https://solarianprogrammer.com/2019/11/16/install-codeblocks-gcc-windows-build-c-cpp-fortran-programs/

	2) Include the header file "olcSoundWaveEngine.h" from a .cpp file in your project.
	3) Add these libraries to "Linker Options": user32 winmm
	4) Set this "Compiler Option": -std=c++17
*/

/*
	Using & Installing On Linux
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GNU Compiler Collection (GCC)
	~~~~~~~~~~~~~~~~~~~~~~~
	1) Include the header file "olcSoundWaveEngine.h" from a .cpp file in your project.
	2) Build with the following command:

		g++ olcSoundWaveEngineExample.cpp -o olcSoundWaveEngineExample -lpulse -lpulse-simple -std=c++17

	3) That's it!
	
*/

/*
	Using in multiple-file projects
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	If you intend to use olcSoundWaveEngine across multiple files, it's important to only have one
	instance of the implementation. This is done using the compiler preprocessor definition: OLC_SOUNDWAVE

	This is defined typically before the header is included in teh translation unit you wish the implementation
	to be associated with. To avoid things getting messy I recommend you create	a file "olcSoundWaveEngine.cpp" 
	and that file includes ONLY the following code:

	#define OLC_SOUNDWAVE
	#include "olcSoundWaveEngine.h"
*/

/*
	0.01: olcPGEX_Sound.h reworked
		  +Changed timekeeping to double, added accuracy fix - Thanks scripticuk
		  +Concept of audio drivers and interface
		  +All internal timing now double precision
		  +All internal sampling now single precsion
		  +Loading form WAV files
		  +LERPed sampling from all buffers
		  +Multi-channel audio support
	0.02: +Support multi-channel wave files
		  +Support for 24-bit wave files
		  +Wave files are now sample rate invariant
		  +Linux PulseAudio Updated
		  +Linux ALSA Updated
		  +WinMM Updated
		  +CMake Compatibility
		  =Fix wave format durations preventing playback
		  =Various bug fixes
*/

#pragma once
#ifndef OLC_SOUNDWAVE_H
#define OLC_SOUNDWAVE_H

///[OLC_HM] INSERT swe_prefix.h STD_INCLUDES

///[OLC_HM] INSERT swe_prefix.h COMPILER_SWITCHES

namespace olc::sound
{

	namespace wave
	{
		///[OLC_HM] INSERT swe_wave_file.h WAVE_FILE_TEMPLATE

		///[OLC_HM] INSERT swe_wave_view.h WAVE_VIEW_TEMPLATE		
	}

	///[OLC_HM] INSERT swe_wave_wave.h WAVE_GENERIC_TEMPLATE

	///[OLC_HM] INSERT swe_wave_engine.h WAVE_ENGINE_H

	namespace driver
	{
		///[OLC_HM] INSERT swe_system_base.h SYSTEM_BASE_H
	}


	namespace synth
	{
		///[OLC_HM] INSERT swe_synth_modular.h SYNTH_MODULAR_H

		///[OLC_HM] INSERT swe_synth_osc.h SYNTH_OSCILLATOR_H
	}


}

///[OLC_HM] INSERT swe_system_winmm.h WINMM_H

///[OLC_HM] INSERT swe_system_sdlmixer.h SDLMIXER_H

///[OLC_HM] INSERT swe_system_alsa.h ALSA_H

///[OLC_HM] INSERT swe_system_pulse.h PULSE_H

#ifdef OLC_SOUNDWAVE
#undef OLC_SOUNDWAVE

namespace olc::sound
{	
	///[OLC_HM] INSERT swe_wave_engine.cpp WAVE_ENGINE_CPP

	namespace driver
	{
		///[OLC_HM] INSERT swe_system_base.cpp SYSTEM_BASE_CPP
	}	

	namespace synth
	{
		///[OLC_HM] INSERT swe_synth_modular.cpp SYNTH_MODULAR_CPP

		///[OLC_HM] INSERT swe_synth_osc.cpp SYNTH_OSCILLATOR_CPP
	}
}



///[OLC_HM] INSERT swe_system_winmm.cpp WINMM_CPP
///[OLC_HM] INSERT swe_system_sdlmixer.cpp SDLMIXER_CPP
///[OLC_HM] INSERT swe_system_alsa.cpp ALSA_CPP
///[OLC_HM] INSERT swe_system_pulse.cpp PULSE_CPP

#endif // OLC_SOUNDWAVE IMPLEMENTATION
#endif // OLC_SOUNDWAVE_H
