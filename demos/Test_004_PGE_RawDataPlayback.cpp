#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "../olcSoundWaveEngine.h"


class Example004 : public olc::PixelGameEngine
{
public:
	Example004()
	{
		sAppName = "Testing Custom Wave Gen";
	}

protected: // Sound Specific
	olc::sound::WaveEngine engine;

	olc::sound::Wave custom_wave;

public:
	bool OnUserCreate() override
	{
		engine.InitialiseAudio(44100, 1);

		custom_wave = olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);

		for (size_t i = 0; i < 44100; i++)
		{
			double dt = 1.0 / 44100.0;
			double t = double(i) * dt;
			custom_wave.file.data()[i] = float(0.5 * sin(2.0 * 440.0 * 3.14159 * t));
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::K1).bPressed)
			engine.PlayWaveform(&custom_wave);

		Clear(olc::BLACK);
		DrawString({ 4,  4 }, "[1] Play 44100Hz x 8-bit Custom Sample");		
		return true;
	}
};



int main()
{
	Example004 demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}