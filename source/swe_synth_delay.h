#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_DELAY_H
	namespace modules
	{
		template<size_t max_ms, size_t samplerate>
		class Delay : public olc::sound::synth::Module {
		public:
			olc::sound::synth::Property input = 0.0;
			olc::sound::synth::Property output = 0.0;
			olc::sound::synth::Property decay = 1.0;
			olc::sound::synth::Property delay = 1.0;
			std::array<double, (max_ms * samplerate) / 1000> state{0.0};
			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) override;
		private:
			double max_delay = static_cast<double>(max_ms) / 1000.0;
			size_t input_index = 0;
			size_t output_index = 1;
		};
	}
	///[OLC_HM] END SYNTH_DELAY_H
}
