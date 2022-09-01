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
			state[input_index] = input.value * decay.value;
			input_index = (input_index + 1) % state.size();

			//Determine where we should sample from based on the desired delay amount
			size_t out_dist = std::min(state.size() - 1, static_cast<size_t>(delay.value * max_delay * samplerate));
			output_index = (input_index - out_dist + state.size()) % state.size();
			output = state[output_index];
		}
	}
	///[OLC_HM] END SYNTH_DELAY_CPP
}
