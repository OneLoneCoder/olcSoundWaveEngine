#pragma once
#include "swe_synth_pinkifier.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_PINKIFIER_CPP
	namespace modules
	{
		void Pinkifier::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			f1.input = input;
			f1.Update(nChannel, dTime, dTimeStep);
			f2.input = f1.output;
			f2.Update(nChannel, dTime, dTimeStep);
			f3.input = f2.output;
			f3.Update(nChannel, dTime, dTimeStep);
			output = f3.output;
		}
	}
	///[OLC_HM] END SYNTH_PINKIFIER_CPP
}