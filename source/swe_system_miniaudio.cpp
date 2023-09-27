#include "swe_system_miniaudio.h"

#include "swe_wave_engine.h"
#include <cassert>

///[OLC_HM] START MINIAUDIO_CPP
#if defined(SOUNDWAVE_USING_MINIAUDIO)
// MINIAUDIO Driver Implementation
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


namespace olc::sound::driver
{
	MiniAudio::MiniAudio(WaveEngine* pHost) : Base(pHost)
	{
	}

	MiniAudio::~MiniAudio()
	{
		Stop();
		Close();
		delete m_pDevice;
	}

	bool MiniAudio::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		if(m_pDevice == nullptr)
			m_pDevice = new ma_device();
		
		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		
		deviceConfig.playback.format   = ma_format_f32;
		deviceConfig.playback.channels = m_pHost->GetChannels();
		deviceConfig.sampleRate        = m_pHost->GetSampleRate();
		deviceConfig.dataCallback      = MiniAudio::data_callback;
		deviceConfig.periodSizeInFrames = m_pHost->GetBlockSampleCount();
		deviceConfig.pUserData         = this;

		if (ma_device_init(NULL, &deviceConfig, m_pDevice) != MA_SUCCESS)
			return false;  // Failed to initialize the device.
		
		return true;
	}

	bool MiniAudio::Start()
	{
		ma_device_start(m_pDevice);
		return true;
	}

	void MiniAudio::Stop()
	{
		ma_device_stop(m_pDevice);
	}

	void MiniAudio::Close()
	{
		ma_device_uninit(m_pDevice);
	}
	
	void MiniAudio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		MiniAudio* pMiniAudio = (MiniAudio*)pDevice->pUserData;
		float* pOutputFloat = (float*)pOutput;

		std::vector<float> vFloatBuffer(pMiniAudio->m_pHost->GetBlockSampleCount() * pMiniAudio->m_pHost->GetChannels(), 0.0f);
		pMiniAudio->GetFullOutputBlock(vFloatBuffer);

		for(int i = 0; i < vFloatBuffer.size(); i++)
			pOutputFloat[i] = vFloatBuffer[i];
	}


} // MiniAudio Driver Implementation
#endif
///[OLC_HM] END PULSE_CPP
