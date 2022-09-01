#pragma once
#include "swe_synth_n_mixer.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_N_MIXER_CPP
	namespace modules
	{
		template<size_t N>
		void Mixer<N>::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			double out = 0.0;
			
			for (size_t i = 0; i < N; i++) {
				out += amplitude[i].value * inputs[i].value;
			}

			output = out / N;
		}
	}
	///[OLC_HM] END SYNTH_N_MIXER_CPP
}