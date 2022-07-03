
#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

olc::sound::WaveEngine engine;

int main()
{
	engine.InitialiseAudio();
	olc::sound::Wave w("./assets/SampleA.wav");
	return 0;
}