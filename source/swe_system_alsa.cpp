#include "swe_system_alsa.h"

#include "swe_wave_engine.h"


///[OLC_HM] START ALSA_CPP
#if defined(SOUNDWAVE_USING_ALSA)
// ALSA Driver Implementation
namespace olc::sound::driver
{
	ALSA::ALSA(WaveEngine* pHost) : Base(pHost)
	{ }

	ALSA::~ALSA()
	{
		Stop();
		Close();
	}

	bool ALSA::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		// Open PCM stream
		int rc = snd_pcm_open(&m_pPCM, "default", SND_PCM_STREAM_PLAYBACK, 0);
		if (rc < 0)
			return false;

		// Prepare the parameter structure and set default parameters
		snd_pcm_hw_params_t *params;
		snd_pcm_hw_params_alloca(&params);
		snd_pcm_hw_params_any(m_pPCM, params);

		// Set other parameters
		snd_pcm_hw_params_set_access(m_pPCM, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(m_pPCM, params, SND_PCM_FORMAT_FLOAT);
		snd_pcm_hw_params_set_rate(m_pPCM, params, m_pHost->GetSampleRate(), 0);
		snd_pcm_hw_params_set_channels(m_pPCM, params, m_pHost->GetChannels());
		snd_pcm_hw_params_set_period_size(m_pPCM, params, m_pHost->GetBlockSampleCount(), 0);
		snd_pcm_hw_params_set_periods(m_pPCM, params, m_pHost->GetBlocks(), 0);

		// Save these parameters
		rc = snd_pcm_hw_params(m_pPCM, params);
		if (rc < 0)
			return false;

		return true;
	}

	bool ALSA::Start()
	{
		// Unsure if really needed, helped prevent underrun on my setup
		std::vector<float> vSilence(m_pHost->GetBlockSampleCount(), 0.0f);
		snd_pcm_start(m_pPCM);
		for (unsigned int i = 0; i < m_pHost->GetBlocks(); i++)
			snd_pcm_writei(m_pPCM, vSilence.data(), m_pHost->GetBlockSampleCount());

		snd_pcm_start(m_pPCM);
		m_bDriverLoopActive = true;
		m_thDriverLoop = std::thread(&ALSA::DriverLoop, this);

		return true;
	}

	void ALSA::Stop()
	{
		// Signal the driver loop to exit
		m_bDriverLoopActive = false;

		// Wait for driver thread to exit gracefully
		if (m_thDriverLoop.joinable())
			m_thDriverLoop.join();
	}

	void ALSA::Close()
	{
		if (m_pPCM != nullptr)
		{
			snd_pcm_close(m_pPCM);
			m_pPCM = nullptr;
		}
	}

	void ALSA::DriverLoop()
	{
		// We will be using this vector to transfer to the host for filling, with
		// user sound data (float32, -1.0 --> +1.0)
		std::vector<float> vFloatBuffer(m_pHost->GetBlockSampleCount(), 0.0f);

		// While the system is active, start requesting audio data
		while (m_bDriverLoopActive)
		{
			// Grab audio data from user
			GetFullOutputBlock(vFloatBuffer);
			snd_pcm_writei(m_pPCM, vFloatBuffer.data(), m_pHost->GetBlockSampleCount());
		}
	}
} // ALSA Driver Implementation
#endif
///[OLC_HM] END ALSA_CPP
