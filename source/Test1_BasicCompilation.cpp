
#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

olc::sound::WaveEngine engine;

int main()
{
	engine.InitialiseAudio(44100, 2);
	olc::sound::Wave w("./assets/SampleA.wav");
	w.file.data()[23] = 8;


	olc::sound::Wave wavCustom(2, sizeof(int16_t), 44100, 44100);
	for (size_t i = 0; i < 44100; i++)
	{
		double dt = 1.0 / 44100.0;
		double t = double(i) * dt;
		wavCustom.file.data()[i * wavCustom.file.channels() + 0] =  float(0.5 * sin(2.0 * 440.0 * 3.14159 * t));
		wavCustom.file.data()[i * wavCustom.file.channels() + 1] =  float(0.5 * sin(2.0 * 800.0 * 3.14159 * t));
	}



	engine.PlayWaveform(&wavCustom);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(5s);


	return 0;
}