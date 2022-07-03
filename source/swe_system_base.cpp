#include "swe_system_base.h"

#include "swe_wave_engine.h"

namespace olc::sound::driver
{
	///[OLC_HM] START SYSTEM_BASE_CPP
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
	///[OLC_HM] END SYSTEM_BASE_CPP
} // AudioDriver Base Implementation