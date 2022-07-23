#include "swe_system_openal.h"

#include "swe_wave_engine.h"


///[OLC_HM] START OPENAL_CPP
#if defined(SOUNDWAVE_USING_OPENAL)
// OPENAL Driver Implementation
namespace olc::sound::driver
{
	OpenAL::OpenAL(WaveEngine* pHost) : Base(pHost)
	{ }

	OpenAL::~OpenAL()
	{
		Stop();
		Close();
	}

	bool OpenAL::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		// Open the device and create the context
		m_pDevice = alcOpenDevice(NULL);
		if (m_pDevice)
		{
			m_pContext = alcCreateContext(m_pDevice, NULL);
			alcMakeContextCurrent(m_pContext);
		}
		else return false;

		// Allocate descriptors for sound buffers
		m_pBuffers = new ALuint[m_pHost->GetBlocks()];
		alGenBuffers(m_pHost->GetBlocks(), m_pBuffers);
		alGenSources(1, &m_nSource);

		// All buffers are available by default
		for (unsigned int i = 0; i < m_pHost->GetBlocks(); i++)
			m_qAvailableBuffers.push(m_pBuffers[i]);

		// Allocate Wave|Block Memory
		m_pBlockMemory = new float[m_pHost->GetBlockSampleCount()]{};
		if (m_pBlockMemory == nullptr)
			return false;

		return true;
	}

	bool OpenAL::Start()
	{
		m_bDriverLoopActive = true;
		m_thDriverLoop = std::thread(&OpenAL::DriverLoop, this);

		return true;
	}

	void OpenAL::Stop()
	{
		m_bDriverLoopActive = false;
		m_thDriverLoop.join();
	}

	void OpenAL::Close()
	{
		alDeleteBuffers(m_pHost->GetBlocks(), m_pBuffers);
		delete[] m_pBuffers;
		alDeleteSources(1, &m_nSource);

		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_pContext);
		alcCloseDevice(m_pDevice);

		m_pBuffers = nullptr;
		m_pContext = nullptr;
		m_pDevice = nullptr;
	}

	void OpenAL::DriverLoop()
	{
		// We will be using this vector to transfer to the host for filling, with
		// user sound data (float32, -1.0 --> +1.0)
		std::vector<float> vFloatBuffer(m_pHost->GetBlockSampleCount(), 0.0f);
		std::vector<ALuint> vProcessed;

		// While the system is active, start requesting audio data
		while (m_bDriverLoopActive)
		{
			ALint nState, nProcessed;
			alGetSourcei(m_nSource, AL_SOURCE_STATE, &nState);
			alGetSourcei(m_nSource, AL_BUFFERS_PROCESSED, &nProcessed);

			// Add processed buffers to our queue
			vProcessed.resize(nProcessed);
			alSourceUnqueueBuffers(m_nSource, nProcessed, vProcessed.data());
			for (ALint nBuf : vProcessed) m_qAvailableBuffers.push(nBuf);

			// Wait until there is a free buffer (ewww)
			if (m_qAvailableBuffers.empty()) continue;

			// Grab audio data from user
			GetFullOutputBlock(vFloatBuffer);

			// Fill OpenAL data buffer
			alBufferData(
				m_qAvailableBuffers.front(),
				m_pHost->GetChannels() == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32,
				vFloatBuffer.data(),
				sizeof(float) * m_pHost->GetBlockSampleCount(),
				m_pHost->GetSampleRate()
			);
			// Add it to the OpenAL queue
			alSourceQueueBuffers(m_nSource, 1, &m_qAvailableBuffers.front());
			// Remove it from ours
			m_qAvailableBuffers.pop();

			// If it's not playing for some reason, change that
			if (nState != AL_PLAYING)
				alSourcePlay(m_nSource);
		}
	}
} // OpenAL Driver Implementation
#endif
///[OLC_HM] END OPENAL_CPP
