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
		int rc = snd_pcm_open(&m_pPCM, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

		// Clear global cache.
		// This won't affect users who don't want to create multiple instances of this driver,
		// but it will prevent valgrind from whining about "possibly lost" memory.
		// If the user's ALSA setup uses a PulseAudio plugin, then valgrind will still compain
		// about some "still reachable" data used by that plugin. TODO?
		snd_config_update_free_global();

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
		std::vector<float> vSilence(m_pHost->GetBlockSampleCount() * m_pHost->GetChannels(), 0.0f);
		snd_pcm_start(m_pPCM);
		for (unsigned int i = 0; i < m_pHost->GetBlocks(); i++)
			snd_pcm_writei(m_pPCM, vSilence.data(), m_pHost->GetBlockSampleCount());

		m_rBuffers.Resize(m_pHost->GetBlocks(), m_pHost->GetBlockSampleCount() * m_pHost->GetChannels());

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

		if (m_pPCM != nullptr)
			snd_pcm_drop(m_pPCM);
	}

	void ALSA::Close()
	{
		if (m_pPCM != nullptr)
		{
			snd_pcm_close(m_pPCM);
			m_pPCM = nullptr;
		}
		// Clear the global cache again for good measure
		snd_config_update_free_global();
	}

	void ALSA::DriverLoop()
	{
		const uint32_t nFrames = m_pHost->GetBlockSampleCount();

		int err;
		std::vector<pollfd> vFDs;

		int nFDs = snd_pcm_poll_descriptors_count(m_pPCM);
		if (nFDs < 0)
		{
			std::cerr << "snd_pcm_poll_descriptors_count returned " << nFDs << "\n";
			std::cerr << "disabling polling\n";
			nFDs = 0;
		}
		else
		{
			vFDs.resize(nFDs);

			err = snd_pcm_poll_descriptors(m_pPCM, vFDs.data(), vFDs.size());
			if (err < 0)
			{
				std::cerr << "snd_pcm_poll_descriptors returned " << err << "\n";
				std::cerr << "disabling polling\n";
				vFDs = {};
			}
		}

		// While the system is active, start requesting audio data
		while (m_bDriverLoopActive)
		{
			if (!m_rBuffers.IsFull())
			{
				// Grab some audio data
				auto& vFreeBuffer = m_rBuffers.GetFreeBuffer();
				GetFullOutputBlock(vFreeBuffer);
			}

			// Wait a bit if our buffer is full
			auto avail = snd_pcm_avail_update(m_pPCM);
			while (m_rBuffers.IsFull() && avail < nFrames)
			{
				if (vFDs.size() == 0) break;

				err = poll(vFDs.data(), vFDs.size(), -1);
				if (err < 0)
					std::cerr << "poll returned " << err << "\n";

				unsigned short revents;
				err = snd_pcm_poll_descriptors_revents(m_pPCM, vFDs.data(), vFDs.size(), &revents);
				if (err < 0)
					std::cerr << "snd_pcm_poll_descriptors_revents returned " << err << "\n";

				if (revents & POLLERR)
					std::cerr << "POLLERR\n";

				avail = snd_pcm_avail_update(m_pPCM);
			}

			// Write whatever we can
			while (!m_rBuffers.IsEmpty() && avail >= nFrames)
			{
				auto vFullBuffer = m_rBuffers.GetFullBuffer();
				uint32_t nWritten = 0;

				while (nWritten < nFrames)
				{
					auto err = snd_pcm_writei(m_pPCM, vFullBuffer.data() + nWritten, nFrames - nWritten);
					if (err > 0)
						nWritten += err;
					else
					{
						std::cerr << "snd_pcm_writei returned " << err << "\n";
						break;
					}
				}
				avail = snd_pcm_avail_update(m_pPCM);
			}
		}
	}
} // ALSA Driver Implementation
#endif
///[OLC_HM] END ALSA_CPP
