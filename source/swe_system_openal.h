#pragma once
#include "swe_system_base.h"

///[OLC_HM] START OPENAL_H
#if defined(SOUNDWAVE_USING_OPENAL)
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <queue>

namespace olc::sound::driver
{
	class OpenAL : public Base
	{
	public:
		OpenAL(WaveEngine* pHost);
		~OpenAL();

	protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;

	private:
		void DriverLoop();

		std::queue<ALuint> m_qAvailableBuffers;
		ALuint *m_pBuffers;
		ALuint m_nSource;
		ALCdevice *m_pDevice;
		ALCcontext *m_pContext;
		float* m_pBlockMemory;
		std::atomic<bool> m_bDriverLoopActive{ false };
		std::thread m_thDriverLoop;
	};
}
#endif // SOUNDWAVE_USING_OPENAL
///[OLC_HM] END OPENAL_H
