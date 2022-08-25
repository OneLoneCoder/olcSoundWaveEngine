#include "swe_system_pulse.h"

#include "swe_wave_engine.h"


///[OLC_HM] START PULSE_CPP
#if defined(SOUNDWAVE_USING_PULSE)
// PULSE Driver Implementation
#include <pulse/error.h>
#include <iostream>

namespace olc::sound::driver
{
	PulseAudio::PulseAudio(WaveEngine* pHost) : Base(pHost)
	{ }

	PulseAudio::~PulseAudio()
	{
		Stop();
		Close();
	}

	bool PulseAudio::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		pa_sample_spec ss {
			PA_SAMPLE_FLOAT32, m_pHost->GetSampleRate(), (uint8_t)m_pHost->GetChannels()
		};

		m_pPA = pa_simple_new(NULL, "olcSoundWaveEngine", PA_STREAM_PLAYBACK, NULL,
		                      "Output Stream", &ss, NULL, NULL, NULL);

		if (m_pPA == NULL)
			return false;

		return true;
	}

	bool PulseAudio::Start()
	{
		m_bDriverLoopActive = true;
		m_thDriverLoop = std::thread(&PulseAudio::DriverLoop, this);

		return true;
	}

	void PulseAudio::Stop()
	{
		// Signal the driver loop to exit
		m_bDriverLoopActive = false;

		// Wait for driver thread to exit gracefully
		if (m_thDriverLoop.joinable())
			m_thDriverLoop.join();
	}

	void PulseAudio::Close()
	{
		if (m_pPA != nullptr)
		{
			pa_simple_free(m_pPA);
			m_pPA = nullptr;
		}
	}

	void PulseAudio::DriverLoop()
	{
		// We will be using this vector to transfer to the host for filling, with
		// user sound data (float32, -1.0 --> +1.0)
		std::vector<float> vFloatBuffer(m_pHost->GetBlockSampleCount() * m_pHost->GetChannels(), 0.0f);

		// While the system is active, start requesting audio data
		while (m_bDriverLoopActive)
		{
			// Grab audio data from user
			GetFullOutputBlock(vFloatBuffer);

			// Fill PulseAudio data buffer
			int error;
			if (pa_simple_write(m_pPA, vFloatBuffer.data(),
			                    vFloatBuffer.size() * sizeof(float), &error) < 0)
			{
				std::cerr << "Failed to feed data to PulseAudio: " << pa_strerror(error) << "\n";
			}
		}
	}
} // PulseAudio Driver Implementation
#endif
///[OLC_HM] END PULSE_CPP
