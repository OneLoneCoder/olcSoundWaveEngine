#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"
#include "swe_synth_first_order_filter.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_PINKIFIER_H
	namespace modules
	{
		class Pinkifier : public olc::sound::synth::Module {
			FirstOrderFilter f1 = { 0.99572754 , 0.98443604};
			FirstOrderFilter f2 = { 0.94790649 , 0.83392334 };
			FirstOrderFilter f3 = { 0.53567505 , 0.07568359 };

		public:
			olc::sound::synth::Property input = 0.0;
			olc::sound::synth::Property output = 0.0;
			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) override;
		};
	}
	///[OLC_HM] END SYNTH_PINKIFIER_H
}