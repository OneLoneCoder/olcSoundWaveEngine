#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "..\olcSoundWaveEngine.h"


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
		sample_44100.LoadAudioWaveform("./assets/Sample_Test_44100.wav");
		sample_48000.LoadAudioWaveform("./assets/Sample_Test_48000.wav");		
		engine.InitialiseAudio(44100,2);		
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
		return true;
	}
};



int main()
{
	Example003 demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}