/*
	+-------------------------------------------------------------+
	|             OneLoneCoder Sound Wave Engine v0.01            |
	|  "You wanted noise? Well is this loud enough?" - javidx9    |
	+-------------------------------------------------------------+

	What is this?
	~~~~~~~~~~~~~
	olc::SoundWaveEngine is a single file, cross platform audio
	interface for lightweight applications that just need a bit of
	easy audio manipulation.

	It's origins started in the olcNoiseMaker file that accompanied
	javidx9's "Code-It-Yourself: Synthesizer" series. It was refactored
	and absorbed into the "olcConsoleGameEngine.h" file, and then
	refactored again into olcPGEX_Sound.h, that was an extension to
	the awesome "olcPixelGameEngine.h" file.

	Alas, it went underused and began to rot, with many myths circulating
	that "it doesnt work" and "it shouldn't be used". These untruths
	made javidx9 feel sorry for the poor file, and he decided to breathe
	some new life into it, in anticipation of new videos!

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2022 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met :

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditionsand the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice.This list of conditions and the following
	disclaimer must be reproduced in the documentation and /or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9

	Thanks
	~~~~~~
	Slavka     - OpenAL and ALSA
	MaGetzUb   - XAudio
	Gorbit99   - Testing, Bug Fixes
	Cyberdroid - Testing, Bug Fixes
	Dragoneye  - Testing
	Puol       - Testing

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019, 2020, 2021, 2022
*/


/*
	1.00: olcPGEX_Sound.h reworked
		  +Changed timekeeping to double, added accuracy fix - Thanks scripticuk
		  +Concept of audio drivers and interface
		  +All internal timing now double precision
		  +All internal sampling now single precsion
		  +Loading form WAV files
		  +LERPed sampling from all buffers
		  +Multi-channel audio support
*/

#pragma once
#ifndef OLC_SOUNDWAVE_H
#define OLC_SOUNDWAVE_H

#include <cstdint>
#include <vector>
#include <list>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <string>
#include <algorithm>
#include <fstream>

// TODO: Compiler/System Sensitivity
#define SOUNDWAVE_USING_WINMM

namespace olc::sound
{

	namespace wave
	{
	// Physically represents a .WAV file, but the data is stored
	// as normalised floating point values
	template<class T = float>
	class File
	{
	public:
		T* data() const
		{
			return m_pRawData.get();
		}

		size_t samples() const
		{
			return m_nSamples;
		}

		size_t channels() const
		{
			return m_nChannels;
		}

		size_t samplesize() const
		{
			return m_nSampleSize;
		}

		size_t samplerate() const
		{
			return m_nSampleRate;
		}

		bool LoadFile(const std::string& sFilename)
		{
			std::ifstream ifs(sFilename, std::ios::binary);
			if (!ifs.is_open())
				return false;

			struct WaveFormatHeader
			{
				uint16_t wFormatTag;         /* format type */
				uint16_t nChannels;          /* number of channels (i.e. mono, stereo...) */
				uint32_t nSamplesPerSec;     /* sample rate */
				uint32_t nAvgBytesPerSec;    /* for buffer estimation */
				uint16_t nBlockAlign;        /* block size of data */
				uint16_t wBitsPerSample;     /* number of bits per sample of mono data */
			};

			WaveFormatHeader header{ 0 };

			m_pRawData.reset();

			char dump[4];
			ifs.read(dump, sizeof(uint8_t) * 4); // Read "RIFF"
			if (strncmp(dump, "RIFF", 4) != 0) return false;

			ifs.read(dump, sizeof(uint8_t) * 4); // Not Interested
			ifs.read(dump, sizeof(uint8_t) * 4); // Read "WAVE"
			if (strncmp(dump, "WAVE", 4) != 0) return false;

			// Read Wave description chunk
			ifs.read(dump, sizeof(uint8_t) * 4); // Read "fmt "
			ifs.read(dump, sizeof(uint8_t) * 4); // Not Interested
			ifs.read((char*)&header, sizeof(WaveFormatHeader)); // Read Wave Format Structure chunk

			// Search for audio data chunk
			int32_t nChunksize = 0;
			ifs.read(dump, sizeof(uint8_t) * 4); // Read chunk header
			ifs.read((char*)&nChunksize, sizeof(uint32_t)); // Read chunk size

			while (strncmp(dump, "data", 4) != 0)
			{
				// Not audio data, so just skip it
				ifs.seekg(nChunksize, std::ios::_Seekcur);
				ifs.read(dump, sizeof(uint8_t) * 4); // Read next chunk header
				ifs.read((char*)&nChunksize, sizeof(uint32_t)); // Read next chunk size
			}

			// Finally got to data, so read it all in and convert to float samples
			m_nSampleSize = header.wBitsPerSample >> 3;
			m_nSamples = nChunksize / (header.nChannels * m_nSampleSize);
			m_nChannels = header.nChannels;
			m_nSampleRate = header.nSamplesPerSec;
			m_pRawData = std::make_unique<T[]>(m_nSamples * m_nChannels);

			T* pSample = m_pRawData.get();

			// Read in audio data and normalise
			for (long i = 0; i < m_nSamples; i++)
			{
				for (int c = 0; c < m_nChannels; c++)
				{
					switch (m_nSampleSize)
					{
					case 1:
					{
						int8_t s = 0;
						ifs.read((char*)&s, sizeof(int8_t));
						*pSample = T(s) / T(std::numeric_limits<int8_t>::max());
					}
					break;

					case 2:
					{
						int16_t s = 0;
						ifs.read((char*)&s, sizeof(int16_t));
						*pSample = T(s) / T(std::numeric_limits<int16_t>::max());
					}
					break;

					case 4:
					{
						int32_t s = 0;
						ifs.read((char*)&s, sizeof(int32_t));
						*pSample = T(s) / T(std::numeric_limits<int32_t>::max());
					}
					break;
					}

					pSample++;
				}
			}
			return true;
		}


	protected:
		std::unique_ptr<T[]> m_pRawData;
		size_t m_nSamples = 0;
		size_t m_nChannels = 0;
		size_t m_nSampleRate = 0;
		size_t m_nSampleSize = 0;
	};

	template<typename T>
	class View
	{
	public:
		View() = default;

		View(const T* pData, const size_t nSamples)
		{
			SetData(pData, nSamples);
		}

	public:
		void SetData(T const* pData, const size_t nSamples, const size_t nStride = 1, const size_t nOffset = 0)
		{
			m_pData = pData;
			m_nSamples = nSamples;
			m_nStride = nStride;
			m_nOffset = nOffset;
		}

		double GetSample(const double dSample) const
		{
			double d1 = std::floor(dSample);
			size_t p1 = static_cast<size_t>(d1);
			size_t p2 = p1 + 1;

			double t = dSample - d1;
			double a = GetValue(p1);
			double b = GetValue(p2);

			return a + t * (b - a); // std::lerp in C++20
		}

		std::pair<double, double> GetRange(const double dSample1, const double dSample2) const
		{
			if (dSample1 < 0 || dSample2 < 0)
				return { 0,0 };

			if (dSample1 > m_nSamples && dSample2 > m_nSamples)
				return { 0,0 };

			double dMin, dMax;

			double d = GetSample(dSample1);
			dMin = dMax = d;

			size_t n1 = static_cast<size_t>(std::ceil(dSample1));
			size_t n2 = static_cast<size_t>(std::floor(dSample2));
			for (size_t n = n1; n < n2; n++)
			{
				d = GetValue(n);
				dMin = std::min(dMin, d);
				dMax = std::max(dMax, d);
			}

			d = GetSample(dSample2);
			dMin = std::min(dMin, d);
			dMax = std::max(dMax, d);

			return { dMin, dMax };
		}

		T GetValue(const size_t nSample) const
		{
			if (nSample >= m_nSamples)
				return 0;
			else
				return m_pData[m_nOffset + nSample * m_nStride];
		}

	private:
		const T* m_pData = nullptr;
		size_t m_nSamples = 0;
		size_t m_nStride = 1;
		size_t m_nOffset = 0;
	};
	}

	template<typename T = float>
	class Wave_generic
	{
	public:
		Wave_generic() = default;
		Wave_generic(std::string sWavFile) { LoadAudioWaveform(sWavFile); }
		Wave_generic(std::istream& sStream) { LoadAudioWaveform(sStream); }
		Wave_generic(const char* pData, const size_t nBytes) { LoadAudioWaveform(pData, nBytes); }

		bool LoadAudioWaveform(std::string sWavFile)
		{
			vChannelView.clear();

			if (file.LoadFile(sWavFile))
			{
				// Setup views for each channel
				vChannelView.resize(file.channels());
				for (uint32_t c = 0; c < file.channels(); c++)
					vChannelView[c].SetData(file.data(), file.samples(), file.channels(), c);

				return true;
			}

			return false;
		}

		bool LoadAudioWaveform(std::istream& sStream) { return false; }
		bool LoadAudioWaveform(const char* pData, const size_t nBytes) { return false; }

		std::vector<wave::View<T>> vChannelView;
		wave::File<T> file;
	};

	typedef Wave_generic<float> Wave;

	struct WaveInstance
	{
		Wave* pWave = nullptr;
		double dInstanceTime = 0.0;
		double dDuration = 0.0;
		double dSpeedModifier = 1.0;
		bool bFinished = false;
		bool bLoop = false;
		bool bFlagForStop = false;
	};

	typedef std::list<WaveInstance>::iterator PlayingWave;

	namespace driver
	{
		class Base;
	}

	// Container class for Basic Sound Manipulation
	class WaveEngine
	{

	public:
		WaveEngine();
		virtual ~WaveEngine();

		// Configure Audio Hardware
		bool InitialiseAudio(uint32_t nSampleRate = 44100, uint32_t nChannels = 1, uint32_t nBlocks = 8, uint32_t nBlockSamples = 512);

		// Release Audio Hardware
		bool DestroyAudio();

		// Call to get the names of all the devices capable of audio output - DACs. An entry
		// from the returned collection can be specified as the device to use in UseOutputDevice()
		std::vector<std::string> GetOutputDevices();

		// Specify a device for audio output prior to calling InitialiseAudio()
		void UseOutputDevice(const std::string& sDeviceOut);

		// Call to get the names of all the devices capable of audio input - ADCs. An entry
		// from the returned collection can be specified as the device to use in UseInputDevice()
		std::vector<std::string> GetInputDevices();

		// Specify a device for audio input prior to calling InitialiseAudio()
		void UseInputDevice(const std::string& sDeviceOut);


		void SetCallBack_SynthFunction(std::function<float(uint32_t, double)> func);
		void SetCallBack_FilterFunction(std::function<float(uint32_t, double, float)> func);

	public:
		void SetOutputVolume(const float fVolume);



		PlayingWave PlayWaveform(Wave* pWave, bool bLoop = false, double dSpeed = 1.0);
		void StopWaveform(const PlayingWave& w);
		void StopAll();

	private:
		uint32_t FillOutputBuffer(std::vector<float>& vBuffer, const uint32_t nBufferOffset, const uint32_t nRequiredSamples);

	private:
		std::unique_ptr<driver::Base> m_driver;
		std::function<float(uint32_t, double)> m_funcUserSynth;
		std::function<float(uint32_t, double, float)> m_funcUserFilter;


	private:
		uint32_t m_nSampleRate = 44100;
		uint32_t m_nChannels = 1;
		uint32_t m_nBlocks = 8;
		uint32_t m_nBlockSamples = 512;
		double	 m_dSamplePerTime = 44100.0;
		double   m_dTimePerSample = 1.0 / 44100;
		double   m_dGlobalTime = 0.0;
		float m_fOutputVolume = 1.0;

		std::string m_sInputDevice;
		std::string m_sOutputDevice;

	private:
		std::list<WaveInstance> m_listWaves;

	public:
		uint32_t GetSampleRate() const;
		uint32_t GetChannels() const;
		uint32_t GetBlocks() const;
		uint32_t GetBlockSampleCount() const;
		double GetTimePerSample() const;


		// Friends, for access to FillOutputBuffer from Drivers
		friend class driver::Base;

	};

	namespace driver
	{
	// DRIVER DEVELOPERS ONLY!!!
	//
	// This interface allows SoundWave to exchange data with OS audio systems. It 
	// is not intended of use by regular users.
	class Base
	{
	public:
		Base(WaveEngine* pHost);
		virtual ~Base();

	public:
		// [IMPLEMENT] Opens a connection to the hardware device, returns true if success
		virtual bool Open(const std::string& sOutputDevice, const std::string& sInputDevice);
		// [IMPLEMENT] Starts a process that repeatedly requests audio, returns true if success
		virtual bool Start();
		// [IMPLEMENT] Stops a process form requesting audio
		virtual void Stop();
		// [IMPLEMENT] Closes any connections to hardware devices
		virtual void Close();

		virtual std::vector<std::string> EnumerateOutputDevices();
		virtual std::vector<std::string> EnumerateInputDevices();

	protected:
		// [IMPLEMENT IF REQUIRED] Called by driver to exchange data with SoundWave System. Your
		// implementation will call this function providing a "DAC" buffer to be filled by
		// SoundWave from a buffer of floats filled by the user.		
		void ProcessOutputBlock(std::vector<float>& vFloatBuffer, std::vector<short>& vDACBuffer);

		// Handle to SoundWave, to interrogate optons, and get user data
		WaveEngine* m_pHost = nullptr;
	};
	}


}

#if defined(SOUNDWAVE_USING_WINMM)
#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

namespace olc::sound::driver
{
	class WinMM : public Base
	{
	public:
		WinMM(WaveEngine* pHost);
		~WinMM();

	protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;

	private:
		void DriverLoop();
		void FreeAudioBlock();
		static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2);
		HWAVEOUT m_hwDevice = nullptr;
		std::thread m_thDriverLoop;
		std::atomic<bool> m_bDriverLoopActive{ false };
		std::unique_ptr<std::vector<short>[]> m_pvBlockMemory;
		std::unique_ptr<WAVEHDR[]> m_pWaveHeaders;
		std::atomic<unsigned int> m_nBlockFree = 0;
		std::condition_variable m_cvBlockNotZero;
		std::mutex m_muxBlockNotZero;
		uint32_t m_nBlockCurrent = 0;
	};
}
#endif // SOUNDWAVE_USING_WINMM


#ifdef OLC_SOUNDWAVE
#undef OLC_SOUNDWAVE

namespace olc::sound
{	
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
	}

	WaveEngine::~WaveEngine()
	{

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
		wi.dSpeedModifier = dSpeed;
		wi.dDuration = double(pWave->file.samples()) / double(pWave->file.samplerate()) / wi.dSpeedModifier;
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
							// OR, sample the waveform form the correct channel
							fSample += float(wave.pWave->vChannelView[nChannel].GetSample(dTimeOffset * m_dSamplePerTime * wave.dSpeedModifier));
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

	namespace driver
	{
	Base::Base(olc::sound::WaveEngine* pHost) : m_pHost(pHost)
	{}

	Base::~Base()
	{}

	bool Base::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
	{
		return false;
	}

	bool Base::Start()
	{
		return false;
	}

	void Base::Stop()
	{
	}

	void Base::Close()
	{
	}

	std::vector<std::string> Base::EnumerateOutputDevices()
	{
		return { "DEFAULT" };
	}

	std::vector<std::string> Base::EnumerateInputDevices()
	{
		return { "NONE" };
	}

	void Base::ProcessOutputBlock(std::vector<float>& vFloatBuffer, std::vector<short>& vDACBuffer)
	{
		constexpr float fMaxSample = float(std::numeric_limits<short>::max());
		constexpr float fMinSample = float(std::numeric_limits<short>::min());

		// So... why not use vFloatBuffer.size()? Well with this implementation
		// we can, but i suspect there may be some platforms that request a
		// specific number of samples per "loop" rather than this block architecture
		uint32_t nSamplesToProcess = m_pHost->GetBlockSampleCount();
		uint32_t nSampleOffset = 0;
		while (nSamplesToProcess > 0)
		{
			uint32_t nSamplesGathered = m_pHost->FillOutputBuffer(vFloatBuffer, nSampleOffset, nSamplesToProcess);

			// Vector is in float32 format, so convert to hardware required format
			for (uint32_t n = 0; n < nSamplesGathered; n++)
			{
				for (uint32_t c = 0; c < m_pHost->GetChannels(); c++)
				{
					size_t nSampleID = nSampleOffset + (n * m_pHost->GetChannels() + c);
					vDACBuffer[nSampleID] = short(std::clamp(vFloatBuffer[nSampleID] * fMaxSample, fMinSample, fMaxSample));
				}
			}

			nSampleOffset += nSamplesGathered;
			nSamplesToProcess -= nSamplesGathered;
		}
	}
	}	
}

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

#endif // OLC_SOUNDWAVE IMPLEMENTATION
#endif // OLC_SOUNDWAVE_H

