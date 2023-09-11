#include "swe_system_miniaudio.h"

#include "swe_wave_engine.h"


///[OLC_HM] START MINIAUDIO_CPP
#if defined(SOUNDWAVE_USING_MINIAUDIO)
// MINIAUDIO Driver Implementation

namespace olc::sound::driver
{
	MiniAudio::MiniAudio(WaveEngine* pHost) : Base(pHost)
	{
		std::cout << "MiniAudio\n";
	}

	MiniAudio::~MiniAudio()
	{
		Stop();
		Close();
	}

	bool MiniAudio::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		return true;
	}

	bool MiniAudio::Start()
	{
		return true;
	}
	
	void MiniAudio::Stop()
	{

	}

	void MiniAudio::Close()
	{

	}

} // MiniAudio Driver Implementation
#endif
///[OLC_HM] END PULSE_CPP
