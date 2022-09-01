#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_N_MIXER_H
	namespace modules
	{
		template<size_t N>
		class Mixer : public Module
		{
		public:
			std::array<Property, N> inputs = {};
			std::array<Property, N> amplitude = {};
			Property output;

			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) override;

		};
	}
	///[OLC_HM] END SYNTH_N_MIXER_H
}