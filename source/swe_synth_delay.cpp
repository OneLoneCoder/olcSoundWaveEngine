#pragma once
#include "swe_synth_delay.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_DELAY_CPP
	namespace modules
	{
		template<size_t max_ms, size_t samplerate>
		void Delay<max_ms, samplerate>::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			for (size_t i = state.size() - 1; i > 0; i--) {
				state[i] = state[i - 1];
			}

			//Based on the delay value, sample from the delay line buffer
			size_t out_index = std::min(state.size() - 1, static_cast<size_t>(delay.value * samplerate));
			state[0] = input.value * decay.value;
			output = state[out_index];
		}
	}
	///[OLC_HM] END SYNTH_DELAY_CPP
}