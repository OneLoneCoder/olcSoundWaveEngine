#include "swe_synth_osc.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_OSCILLATOR_CPP
	namespace modules
	{		
		void Oscillator::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			// We use phase accumulation to combat change in parameter glitches
			double w = frequency.value * max_frequency * 2.0 * 3.14159 * dTimeStep;
			phase_acc += w + lfo_input.value * frequency.value;
			if (phase_acc >= 2.0 * 3.14159) phase_acc -= 2.0 * 3.14159;


			
			output = amplitude.value * sin(phase_acc);
		}
	}
	///[OLC_HM] END SYNTH_OSCILLATOR_CPP
}