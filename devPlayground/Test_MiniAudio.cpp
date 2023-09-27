#include "swe_synth_envelope.h"
#include "swe_system_alsa.h"
#include "swe_system_openal.h"
#include "swe_system_wasapi.h"
#include "swe_wave_engine.h"
#include "swe_wave_wave.h"
#include "swe_synth_modular.h"
#include "swe_system_base.h"
#include "swe_system_pulse.h"
#include "swe_system_winmm.h"
#include "swe_wave_file.h"
#include "swe_synth_osc.h"
#include "swe_system_miniaudio.h"
#include "swe_system_sdlmixer.h"
#include "swe_wave_view.h"


#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Example003 : public olc::PixelGameEngine
{
public:
	Example003()
	{
		sAppName = "Testing Sample Rates";
	}

protected: // Sound Specific
	olc::sound::WaveEngine engine;

	olc::sound::Wave sample_44100;
	olc::sound::Wave sample_48000;

	
public:
	bool OnUserCreate() override
	{
		sample_44100.LoadAudioWaveform("../demos/assets/Sample_Test_44100.wav");
		sample_48000.LoadAudioWaveform("../demos/assets/Sample_Test_48000.wav");
		
		// Chromium is notorious for having issues with the audio buffer size and stuttering.
		// From some quick experimenting, anecdotal, I found audio stabilized in Chromium by
		// doubling the buffer. This will need adjusting if sample rate changes, etc...
		// 
		// For 44100, a buffer of size 1024 is ~23ms of buffer

		// (44100 / 1000) * 23 = 1014.3, i rounded up to 1024 because it's an easy to remember number.
		#if defined(__EMSCRIPTEN__)
		engine.InitialiseAudio(44100, 1, 8, 1024);
		#else
		engine.InitialiseAudio();
		#endif
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{		
		if (GetKey(olc::Key::K1).bPressed)
			engine.PlayWaveform(&sample_44100);
		
		if (GetKey(olc::Key::K2).bPressed)
			engine.PlayWaveform(&sample_48000);


		Clear(olc::BLACK);
		DrawString({ 4,  4 }, "[1] Play 44100Hz x 16-bit Sample");
		DrawString({ 4, 14 }, "[2] Play 48000Hz x 24-bit Sample");		
		return !GetKey(olc::ESCAPE).bPressed;
	}
};



int main()
{
	Example003 demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}
