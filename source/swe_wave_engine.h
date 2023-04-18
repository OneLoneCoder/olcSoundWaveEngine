#pragma once

#include "swe_prefix.h"
#include "swe_wave_wave.h"
#include "swe_system_base.h"
#include "swe_system_winmm.h"
#include "swe_system_wasapi.h"
#include "swe_system_openal.h"
#include "swe_system_alsa.h"
#include "swe_system_sdlmixer.h"

namespace olc::sound
{
	///[OLC_HM] START WAVE_ENGINE_H
	struct WaveInstance
	{
		Wave* pWave = nullptr;
		double dInstanceTime = 0.0;
		double dDuration = 0.0;
		double dSpeedModifier = 1.0;
		bool bFinished = false;
		bool bLoop = false;
		bool bFlagForStop = false;
		bool bPaused = false;
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


		void SetCallBack_NewSample(std::function<void(double)> func);
		void SetCallBack_SynthFunction(std::function<float(uint32_t, double)> func);
		void SetCallBack_FilterFunction(std::function<float(uint32_t, double, float)> func);

	public:
		void SetOutputVolume(const float fVolume);



		PlayingWave PlayWaveform(Wave* pWave, bool bLoop = false, double dSpeed = 1.0);
		void PauseWaveform(const PlayingWave& w);
		void ResumeWaveform(const PlayingWave& w);
		void RewindWaveform(const PlayingWave& w);
		void StopWaveform(const PlayingWave& w);
		void StopAll();

	private:
		uint32_t FillOutputBuffer(std::vector<float>& vBuffer, const uint32_t nBufferOffset, const uint32_t nRequiredSamples);

	private:
		std::unique_ptr<driver::Base> m_driver;
		std::function<void(double)> m_funcNewSample;
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
	///[OLC_HM] END WAVE_ENGINE_H
}
