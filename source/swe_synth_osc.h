#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_OSCILLATOR_H
	namespace modules
	{
		class Oscillator : public Module
		{
		public:
			// Primary frequency of oscillation
			Property frequency = 0.0f;
			// Primary amplitude of output
			Property amplitude = 1.0f;
			// LFO input if required
			Property lfo_input = 0.0f;
			// Primary Output
			Property output;

		private:
			double phase_acc = 0.0f;
			double max_frequency = 20000.0;

		public:
			void Update(uint32_t nChannel, double dTime, double dTimeStep) override;

		};
	}
	///[OLC_HM] END SYNTH_OSCILLATOR_H
}