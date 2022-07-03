#include "swe_system_winmm.h"

#include "swe_wave_engine.h"


///[OLC_HM] START WINMM_CPP
#if defined(SOUNDWAVE_USING_WINMM)
// WinMM Driver Implementation
namespace olc::sound::driver
{
	#pragma comment(lib, "winmm.lib")

	WinMM::WinMM(WaveEngine* pHost) : Base(pHost)
	{ }

	WinMM::~WinMM()
	{
		Stop();
		Close();
	}

	bool WinMM::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		// Device is available
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = m_pHost->GetSampleRate();
		waveFormat.wBitsPerSample = sizeof(short) * 8;
		waveFormat.nChannels = m_pHost->GetChannels();
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Open Device if valid
		if (waveOutOpen(&m_hwDevice, WAVE_MAPPER, &waveFormat, (DWORD_PTR)WinMM::waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
			return false;

		// Allocate array of wave header objects, one per block
		m_pWaveHeaders = std::make_unique<WAVEHDR[]>(m_pHost->GetBlocks());

		// Allocate block memory - I dont like vector of vectors, so going with this mess instead
		// My std::vector's content will change, but their size never will - they are basically array now
		m_pvBlockMemory = std::make_unique<std::vector<short>[]>(m_pHost->GetBlocks());
		for (size_t i = 0; i < m_pHost->GetBlocks(); i++)
			m_pvBlockMemory[i].resize(m_pHost->GetBlockSampleCount(), 0);

		// Link headers to block memory - clever, so we only move headers about
		// rather than memory...
		for (unsigned int n = 0; n < m_pHost->GetBlocks(); n++)
		{
			m_pWaveHeaders[n].dwBufferLength = DWORD(m_pvBlockMemory[0].size() * sizeof(short));
			m_pWaveHeaders[n].lpData = (LPSTR)(m_pvBlockMemory[n].data());
		}

		// To begin with, all blocks are free
		m_nBlockFree = m_pHost->GetBlocks();
		return true;
	}

	bool WinMM::Start()
	{
		// Prepare driver thread for activity
		m_bDriverLoopActive = true;
		// and get it going!
		m_thDriverLoop = std::thread(&WinMM::DriverLoop, this);
		return true;
	}

	void WinMM::Stop()
	{
		// Signal the driver loop to exit
		m_bDriverLoopActive = false;

		// Wait for driver thread to exit gracefully
		if (m_thDriverLoop.joinable())
			m_thDriverLoop.join();
	}

	void WinMM::Close()
	{
		waveOutClose(m_hwDevice);
	}

	// Static Callback wrapper - specific instance is specified
	void CALLBACK WinMM::waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		// All sorts of messages may be pinged here, but we're only interested
		// in audio block is finished...
		if (uMsg != WOM_DONE) return;

		// ...which has happened so allow driver object to free resource
		WinMM* driver = (WinMM*)dwInstance;
		driver->FreeAudioBlock();
	}

	void WinMM::FreeAudioBlock()
	{
		// Audio subsystem is done with the block it was using, thus
		// making it available again
		m_nBlockFree++;

		// Signal to driver loop that a block is now available. It 
		// could have been suspended waiting for one
		std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
		m_cvBlockNotZero.notify_one();
	}

	void WinMM::DriverLoop()
	{
		// We will be using this vector to transfer to the host for filling, with 
		// user sound data (float32, -1.0 --> +1.0)
		std::vector<float> vFloatBuffer(m_pHost->GetBlockSampleCount(), 0.0f);

		// While the system is active, start requesting audio data
		while (m_bDriverLoopActive)
		{
			// Are there any blocks available to fill? ...
			if (m_nBlockFree == 0)
			{
				// ...no, So wait until one is available
				std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
				while (m_nBlockFree == 0) // sometimes, Windows signals incorrectly
				{
					// This thread will suspend until this CV is signalled
					// from FreeAudioBlock.
					m_cvBlockNotZero.wait(lm);
				}
			}

			// ...yes, so use next one, by indicating one fewer
			// block is available
			m_nBlockFree--;

			// Prepare block for processing, by oddly, marking it as unprepared :P
			if (m_pWaveHeaders[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
			}

			// Give the userland the opportunity to fill the buffer. Note that the driver
			// doesnt give a hoot about timing. Thats up to the SoundWave interface to 
			// maintain

			// Userland will populate a float buffer, that gets cleanly converted to
			// a buffer of shorts for DAC
			ProcessOutputBlock(vFloatBuffer, m_pvBlockMemory[m_nBlockCurrent]);

			// Send block to sound device
			waveOutPrepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
			waveOutWrite(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
			m_nBlockCurrent++;
			m_nBlockCurrent %= m_pHost->GetBlocks();
		}
	}
} // WinMM Driver Implementation
#endif
///[OLC_HM] END WINMM_CPP