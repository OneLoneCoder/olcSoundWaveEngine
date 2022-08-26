#include "swe_synth_osc.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_OSCILLATOR_CPP
	namespace modules
	{		
		void Oscillator::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			// We use phase accumulation to combat change in parameter glitches
			double w = frequency.value * max_frequency * dTimeStep;
			phase_acc += w + lfo_input.value * frequency.value;
			if (phase_acc >= 2.0) phase_acc -= 2.0;

			switch (waveform)
			{
			case Type::Sine:
				output = amplitude.value * sin(phase_acc * 2.0 * 3.14159);
				break;

			case Type::Saw:
				output = amplitude.value * (phase_acc - 1.0) * 2.0;
				break;

			case Type::Square:
				output = amplitude.value * ((phase_acc >= 1.0) ? 1.0 : -1.0);
				break;

			case Type::Triangle:
				output = amplitude.value * ((phase_acc < 1.0) ? (phase_acc * 0.5) : (1.0 - phase_acc * 0.5));
				break;

			case Type::PWM:
				output = amplitude.value * ((phase_acc >= (parameter.value + 1.0)) ? 1.0 : -1.0);
				break;

			case Type::Wave:
				if(pWave != nullptr)
					output = amplitude.value * pWave->vChannelView[nChannel].GetSample(phase_acc * 0.5 * pWave->file.durationInSamples());
				break;

			case Type::Noise:
				output = amplitude.value * rndDouble(-1.0, 1.0);
				break;

			}
		}

		double Oscillator::rndDouble(double min, double max)
		{
			return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
		}

		uint32_t Oscillator::rnd()
		{
			random_seed += 0xe120fc15;
			uint64_t tmp;
			tmp = (uint64_t)random_seed * 0x4a39b70d;
			uint32_t m1 = uint32_t(((tmp >> 32) ^ tmp) & 0xFFFFFFFF);
			tmp = (uint64_t)m1 * 0x12fad5c9;
			uint32_t m2 = uint32_t(((tmp >> 32) ^ tmp) & 0xFFFFFFFF);
			return m2;
		}
	}
	///[OLC_HM] END SYNTH_OSCILLATOR_CPP
}