#pragma once
#include "swe_system_base.h"

///[OLC_HM] START PULSE_H
#if defined(SOUNDWAVE_USING_PULSE)
#include <pulse/simple.h>

namespace olc::sound::driver
{
	class PulseAudio : public Base
	{
	public:
		PulseAudio(WaveEngine* pHost);
		~PulseAudio();

	protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;

	private:
		void DriverLoop();

		pa_simple *m_pPA;
		std::atomic<bool> m_bDriverLoopActive{ false };
		std::thread m_thDriverLoop;
	};
}
#endif // SOUNDWAVE_USING_PULSE
///[OLC_HM] END PULSE_H
