#include "swe_wave_engine.h"

namespace olc::sound
{
	///[OLC_HM] START WAVE_ENGINE_CPP
	WaveEngine::WaveEngine()
	{
		m_sInputDevice = "NONE";
		m_sOutputDevice = "DEFAULT";

#if defined(SOUNDWAVE_USING_WINMM)
		m_driver = std::make_unique<driver::WinMM>(this);
#endif

#if defined(SOUNDWAVE_USING_WASAPI)
		m_driver = std::make_unique<driver::WASAPI>(this);
#endif

#if defined(SOUNDWAVE_USING_XAUDIO)
		m_driver = std::make_unique<driver::XAudio>(this);
#endif

#if defined(SOUNDWAVE_USING_OPENAL)
		m_driver = std::make_unique<driver::OpenAL>(this);
#endif

#if defined(SOUNDWAVE_USING_ALSA)
		m_driver = std::make_unique<driver::ALSA>(this);
#endif

#if defined(SOUNDWAVE_USING_SDLMIXER)
		m_driver = std::make_unique<driver::SDLMixer>(this);
#endif

#if defined(SOUNDWAVE_USING_PULSE)
		m_driver = std::make_unique<driver::PulseAudio>(this);
#endif

#if defined(SOUNDWAVE_USING_MINIAUDIO)
		m_driver = std::make_unique<driver::MiniAudio>(this);
#endif

	}

	WaveEngine::~WaveEngine()
	{
		DestroyAudio();
	}

	std::vector<std::string> WaveEngine::GetOutputDevices()
	{
		return { "XXX" };
	}


	void WaveEngine::UseOutputDevice(const std::string& sDeviceOut)
	{
		m_sOutputDevice = sDeviceOut;
	}

	std::vector<std::string> WaveEngine::GetInputDevices()
	{
		return { "XXX" };
	}

	void WaveEngine::UseInputDevice(const std::string& sDeviceIn)
	{
		m_sInputDevice = sDeviceIn;
	}

	bool WaveEngine::InitialiseAudio(uint32_t nSampleRate, uint32_t nChannels, uint32_t nBlocks, uint32_t nBlockSamples)
	{
		m_nSampleRate = nSampleRate;
		m_nChannels = nChannels;
		m_nBlocks = nBlocks;
		m_nBlockSamples = nBlockSamples;
		m_dSamplePerTime = double(nSampleRate);
		m_dTimePerSample = 1.0 / double(nSampleRate);
		m_driver->Open(m_sOutputDevice, m_sInputDevice);
		m_driver->Start();
		return false;
	}


	bool WaveEngine::DestroyAudio()
	{
		StopAll();
		m_driver->Stop();
		m_driver->Close();
		return false;
	}

	void WaveEngine::SetCallBack_NewSample(std::function<void(double)> func)
	{
		m_funcNewSample = func;
	}

	void WaveEngine::SetCallBack_SynthFunction(std::function<float(uint32_t, double)> func)
	{
		m_funcUserSynth = func;
	}

	void WaveEngine::SetCallBack_FilterFunction(std::function<float(uint32_t, double, float)> func)
	{
		m_funcUserFilter = func;
	}

	PlayingWave WaveEngine::PlayWaveform(Wave* pWave, bool bLoop, double dSpeed)
	{
		WaveInstance wi;
		wi.bLoop = bLoop;
		wi.pWave = pWave;
		wi.dSpeedModifier = dSpeed * double(pWave->file.samplerate()) / m_dSamplePerTime;
		wi.dDuration = pWave->file.duration() / dSpeed;
		wi.dInstanceTime = m_dGlobalTime;
		m_listWaves.push_back(wi);
		return std::prev(m_listWaves.end());
	}

	void WaveEngine::StopWaveform(const PlayingWave& w)
	{
		w->bFlagForStop = true;
	}

	void WaveEngine::StopAll()
	{
		for (auto& wave : m_listWaves)
		{
			wave.bFlagForStop = true;
		}
	}

	void WaveEngine::SetOutputVolume(const float fVolume)
	{
		m_fOutputVolume = std::clamp(fVolume, 0.0f, 1.0f);
	}

	uint32_t WaveEngine::FillOutputBuffer(std::vector<float>& vBuffer, const uint32_t nBufferOffset, const uint32_t nRequiredSamples)
	{
		for (uint32_t nSample = 0; nSample < nRequiredSamples; nSample++)
		{
			double dSampleTime = m_dGlobalTime + nSample * m_dTimePerSample;

			if (m_funcNewSample)
				m_funcNewSample(dSampleTime);

			for (uint32_t nChannel = 0; nChannel < m_nChannels; nChannel++)
			{
				// Construct the sample
				float fSample = 0.0f;

				// 1) Sample any active waves
				for (auto& wave : m_listWaves)
				{
					// Is wave instance flagged for stopping?
					if (wave.bFlagForStop)
					{
						wave.bFinished = true;
					}
					else
					{
						// Calculate offset into wave instance
						double dTimeOffset = dSampleTime - wave.dInstanceTime;

						// If offset is larger than wave then...
						if (dTimeOffset >= wave.dDuration)
						{
							if (wave.bLoop)
							{
								// ...if looping, reset the wave instance
								wave.dInstanceTime = dSampleTime;
							}
							else
							{
								// ...if not looping, flag wave instance as dead
								wave.bFinished = true;
							}
						}
						else
						{
							// OR, sample the waveform from the correct channel
							fSample += float(wave.pWave->vChannelView[nChannel % wave.pWave->file.channels()].GetSample(dTimeOffset * m_dSamplePerTime * wave.dSpeedModifier));
						}
					}
				}

				// Remove waveform instances that have finished
				m_listWaves.remove_if([](const WaveInstance& wi) {return wi.bFinished; });


				// 2) If user is synthesizing, request sample
				if (m_funcUserSynth)
					fSample += m_funcUserSynth(nChannel, dSampleTime);

				// 3) Apply global filters


				// 4) If user is filtering, allow manipulation of output
				if (m_funcUserFilter)
					fSample = m_funcUserFilter(nChannel, dSampleTime, fSample);

				// Place sample in buffer
				vBuffer[nBufferOffset + nSample * m_nChannels + nChannel] = fSample * m_fOutputVolume;
			}
		}

		// UPdate global time, accounting for error (thanks scripticuk)
		m_dGlobalTime += nRequiredSamples * m_dTimePerSample;
		return nRequiredSamples;
	}


	uint32_t WaveEngine::GetSampleRate() const
	{
		return m_nSampleRate;
	}

	uint32_t WaveEngine::GetChannels() const
	{
		return m_nChannels;
	}

	uint32_t WaveEngine::GetBlocks() const
	{
		return m_nBlocks;
	}

	uint32_t WaveEngine::GetBlockSampleCount() const
	{
		return m_nBlockSamples;
	}

	double WaveEngine::GetTimePerSample() const
	{
		return m_dTimePerSample;
	}
	///[OLC_HM] END WAVE_ENGINE_CPP
} // SoundWave Interface Implementation - Driver agnostic
