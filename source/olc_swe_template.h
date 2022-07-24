/*
	+-------------------------------------------------------------+
	|             OneLoneCoder Sound Wave Engine v0.01            |
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
	Slavka     - OpenAL and ALSA
	MaGetzUb   - XAudio
	Gorbit99   - Testing, Bug Fixes
	Cyberdroid - Testing, Bug Fixes
	Dragoneye  - Testing
	Puol       - Testing

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019, 2020, 2021, 2022
*/


/*
	1.00: olcPGEX_Sound.h reworked
		  +Changed timekeeping to double, added accuracy fix - Thanks scripticuk
		  +Concept of audio drivers and interface
		  +All internal timing now double precision
		  +All internal sampling now single precsion
		  +Loading form WAV files
		  +LERPed sampling from all buffers
		  +Multi-channel audio support
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

///[OLC_HM] INSERT swe_system_alsa.h ALSA_H

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
///[OLC_HM] INSERT swe_system_alsa.cpp ALSA_CPP

#endif // OLC_SOUNDWAVE IMPLEMENTATION
#endif // OLC_SOUNDWAVE_H
