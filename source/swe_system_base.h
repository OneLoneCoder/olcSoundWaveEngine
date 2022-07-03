#pragma once

#include "swe_prefix.h"

namespace olc::sound
{
	class WaveEngine;
}

namespace olc::sound::driver
{
	///[OLC_HM] START SYSTEM_BASE_H
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
	///[OLC_HM] END SYSTEM_BASE_H
} // End Driver Declaration

