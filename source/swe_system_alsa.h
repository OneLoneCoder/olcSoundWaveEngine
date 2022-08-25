#pragma once
#include "swe_system_base.h"

///[OLC_HM] START ALSA_H
#if defined(SOUNDWAVE_USING_ALSA)
#include <alsa/asoundlib.h>
#include <poll.h>
#include <iostream>

namespace olc::sound::driver
{
	// Not thread-safe
	template<typename T>
	class RingBuffer
	{
	public:
		RingBuffer()
		{ }

		void Resize(unsigned int bufnum = 0, unsigned int buflen = 0)
		{
			m_vBuffers.resize(bufnum);
			for (auto &vBuffer : m_vBuffers)
				vBuffer.resize(buflen);
		}

		std::vector<T>& GetFreeBuffer()
		{
			assert(!IsFull());

			std::vector<T>& result = m_vBuffers[m_nTail];
			m_nTail = Next(m_nTail);
			return result;
		}

		std::vector<T>& GetFullBuffer()
		{
			assert(!IsEmpty());

			std::vector<T>& result = m_vBuffers[m_nHead];
			m_nHead = Next(m_nHead);
			return result;
		}

		bool IsEmpty()
		{
			return m_nHead == m_nTail;
		}

		bool IsFull()
		{
			return m_nHead == Next(m_nTail);
		}

	private:
		unsigned int Next(unsigned int current)
		{
			return (current + 1) % m_vBuffers.size();
		}

		std::vector<std::vector<T>> m_vBuffers;
		unsigned int m_nHead = 0;
		unsigned int m_nTail = 0;
	};

	class ALSA : public Base
	{
	public:
		ALSA(WaveEngine* pHost);
		~ALSA();

	protected:
		bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) 	override;
		bool Start() 	override;
		void Stop()		override;
		void Close()	override;

	private:
		void DriverLoop();

		snd_pcm_t *m_pPCM;
		RingBuffer<float> m_rBuffers;
		std::atomic<bool> m_bDriverLoopActive{ false };
		std::thread m_thDriverLoop;
	};
}
#endif // SOUNDWAVE_USING_ALSA
///[OLC_HM] END ALSA_H
