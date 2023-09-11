#pragma once
#include "swe_system_base.h"

///[OLC_HM] START MINIAUDIO_H
#if defined(SOUNDWAVE_USING_MINIAUDIO)

namespace olc::sound::driver
{
	class MiniAudio : public Base
	{
	public:
        MiniAudio(WaveEngine* pHost);
        ~MiniAudio();
	
    protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;
    };
}
#endif // SOUNDWAVE_USING_MINIAUDIO
///[OLC_HM] END MINIAUDIO_H
