#pragma once

#include "swe_prefix.h"
#include "swe_wave_view.h"
#include "swe_wave_file.h"

namespace olc::sound
{
	///[OLC_HM] START WAVE_GENERIC_TEMPLATE
	template<typename T = float>
	class Wave_generic
	{
	public:
		Wave_generic() = default;
		Wave_generic(std::string sWavFile) { LoadAudioWaveform(sWavFile); }
		Wave_generic(std::istream& sStream) { LoadAudioWaveform(sStream); }
		Wave_generic(const char* pData, const size_t nBytes) { LoadAudioWaveform(pData, nBytes); }

		Wave_generic(const size_t nChannels, const size_t nSampleSize, const size_t nSampleRate, const size_t nSamples)
		{
			vChannelView.clear();
			file = wave::File<T>(nChannels, nSampleSize, nSampleRate, nSamples);
			vChannelView.resize(file.channels());
			for (uint32_t c = 0; c < file.channels(); c++)
				vChannelView[c].SetData(file.data(), file.samples(), file.channels(), c);
		}

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
	///[OLC_HM] END WAVE_GENERIC_TEMPLATE
}