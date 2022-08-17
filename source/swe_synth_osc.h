#pragma once
#include "swe_prefix.h"
#include "swe_synth_modular.h"
#include "swe_wave_wave.h"

namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_OSCILLATOR_H
	namespace modules
	{
		class Oscillator : public Module
		{
		public:
			enum class Type
			{
				Sine,
				Saw,
				Square,
				Triangle,
				PWM,
				Wave,
				Noise,
			};

		public:
			// Primary frequency of oscillation
			Property frequency = 0.0f;
			// Primary amplitude of output
			Property amplitude = 1.0f;
			// LFO input if required
			Property lfo_input = 0.0f;
			// Primary Output
			Property output;
			// Tweakable Parameter
			Property parameter = 0.0;

			Type waveform = Type::Sine;

			Wave* pWave = nullptr;

		private:
			double phase_acc = 0.0f;
			double max_frequency = 20000.0;
			uint32_t random_seed = 0xB00B1E5;

			double rndDouble(double min, double max);
			uint32_t rnd();
			

		public:
			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) override;

		};
	}
	///[OLC_HM] END SYNTH_OSCILLATOR_H
}