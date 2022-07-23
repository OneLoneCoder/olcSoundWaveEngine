#pragma once
#include "swe_system_base.h"

///[OLC_HM] START ALSA_H
#if defined(SOUNDWAVE_USING_ALSA)
#include <alsa/asoundlib.h>

namespace olc::sound::driver
{
	class ALSA : public Base
	{
	public:
		ALSA(WaveEngine* pHost);
		~ALSA();

	protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;

	private:
		void DriverLoop();

		snd_pcm_t *m_pPCM;
		std::unique_ptr<float[]> m_pBlockMemory;
		std::atomic<bool> m_bDriverLoopActive{ false };
		std::thread m_thDriverLoop;
	};
}
#endif // SOUNDWAVE_USING_ALSA
///[OLC_HM] END ALSA_H
