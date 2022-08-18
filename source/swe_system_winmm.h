#pragma once
#include "swe_system_base.h"

///[OLC_HM] START WINMM_H
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
///[OLC_HM] END WINMM_H
