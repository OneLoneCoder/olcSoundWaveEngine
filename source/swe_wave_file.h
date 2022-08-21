#pragma once

#include "swe_prefix.h"

// Templates for manipulating wave data
namespace olc::sound::wave
{
	///[OLC_HM] START WAVE_FILE_TEMPLATE
	// Physically represents a .WAV file, but the data is stored
	// as normalised floating point values
	template<class T = float>
	class File
	{
	public:
		File() = default;

		File(const size_t nChannels, const size_t nSampleSize, const size_t nSampleRate, const size_t nSamples)
		{
			m_nChannels = nChannels;
			m_nSampleSize = nSampleSize;
			m_nSamples = nSamples;
			m_nSampleRate = nSampleRate;

			m_pRawData = std::make_unique<T[]>(m_nSamples * m_nChannels);
		}

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

		double duration() const
		{
			return m_dDuration;
		}

		double durationInSamples() const
		{
			return m_dDurationInSamples;
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
				ifs.seekg(nChunksize, std::ios::cur);
				ifs.read(dump, sizeof(uint8_t) * 4); // Read next chunk header
				ifs.read((char*)&nChunksize, sizeof(uint32_t)); // Read next chunk size
			}

			// Finally got to data, so read it all in and convert to float samples
			m_nSampleSize = header.wBitsPerSample >> 3;
			m_nSamples = nChunksize / (header.nChannels * m_nSampleSize);
			m_nChannels = header.nChannels;
			m_nSampleRate = header.nSamplesPerSec;
			m_pRawData = std::make_unique<T[]>(m_nSamples * m_nChannels);			
			m_dDuration =  double(m_nSamples) / double(m_nSampleRate);
			m_dDurationInSamples = double(m_nSamples);

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

					case 3: // 24-bit
					{
						int32_t s = 0;
						ifs.read((char*)&s, 3);
						if (s & (1 << 23)) s |= 0xFF000000;
						*pSample = T(s) / T(std::pow(2, 23)-1);
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

		bool SaveFile(const std::string& sFilename)
		{
			return false;
		}


	protected:
		std::unique_ptr<T[]> m_pRawData;
		size_t m_nSamples = 0;
		size_t m_nChannels = 0;
		size_t m_nSampleRate = 0;
		size_t m_nSampleSize = 0;
		double m_dDuration = 0.0;
		double m_dDurationInSamples = 0.0;
	};
	///[OLC_HM] END WAVE_FILE_TEMPLATE
}