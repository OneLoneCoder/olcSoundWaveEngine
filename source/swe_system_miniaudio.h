#pragma once
#include "swe_system_base.h"

///[OLC_HM] START MINIAUDIO_H
#if defined(SOUNDWAVE_USING_MINIAUDIO)

#include "miniaudio.h"

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
    
    public:
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        
    private:
        ma_device* m_pDevice = nullptr;

    };
}
#endif // SOUNDWAVE_USING_MINIAUDIO
///[OLC_HM] END MINIAUDIO_H