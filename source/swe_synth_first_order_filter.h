#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_FIRST_ORDER_FILTER_H
	namespace modules
	{
		class FirstOrderFilter : public olc::sound::synth::Module {
		public:
			FirstOrderFilter(double p, double z) : pole(p), zero(z) {};
			olc::sound::synth::Property pole;
			olc::sound::synth::Property zero;
			olc::sound::synth::Property state = 0.0;
			olc::sound::synth::Property input = 0.0;
			olc::sound::synth::Property output = 0.0;
			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) override;
		};
	}
	///[OLC_HM] END SYNTH_FIRST_ORDER_FILTER_H
}