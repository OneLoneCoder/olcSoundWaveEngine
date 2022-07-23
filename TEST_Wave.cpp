#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

#include <chrono>
#include <thread>

static float SynthFunction(int nChannel, double dGlobalTime)
{
	const float fSynthFrequency = 277.18263f;

	// Just generate a sine wave of the appropriate frequency
	return std::min(
		fmod(double(fSynthFrequency * 0.1f * 3.14159f) * dGlobalTime, 1.0),
		1.0 - fmod(double(fSynthFrequency * 0.1f * 3.14159f) * dGlobalTime, 1.0)) * 4.0;
}

float FilterFunction(int nChannel, double dGlobalTime, float fSample)
{
	// Fundamentally just control volume
	//float fOutput = fSample * fFilterVolume;

	return fSample;
}

int main()
{
	olc::sound::WaveEngine sound;

	sound.InitialiseAudio();
	sound.SetCallBack_SynthFunction(SynthFunction);
	//sound.SetCallBack_FilterFunction(FilterFunction);

	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
