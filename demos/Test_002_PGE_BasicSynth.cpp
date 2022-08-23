#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "../olcSoundWaveEngine.h"


class BasicSynthesizer : public olc::PixelGameEngine
{
public:
	BasicSynthesizer()
	{
		sAppName = "Basic Synthesizer";
	}

protected: // Sound Specific
	olc::sound::WaveEngine engine;

	olc::sound::Wave sample1;

	// Make a modular synthesizer!
	olc::sound::synth::ModularSynth synth;
	olc::sound::synth::modules::Oscillator osc1;
	olc::sound::synth::modules::Oscillator osc2;
	olc::sound::synth::modules::Oscillator osc3;

	// Called once per device "whole sample" i.e. all channels
	// Useful for sequencing, controlling and visualisation
	void Synthesizer_OnNewCycleRequest(double dTime)
	{
		synth.UpdatePatches();
	}

	// Called individually per sample per channel
	// Useful for actual sound synthesis
	float Synthesizer_OnGetSample(uint32_t nChannel, double dTime)
	{
		synth.Update(nChannel, dTime, 1.0 / 44100.0);		
		return osc1.output.value;
	}
	
public:
	bool OnUserCreate() override
	{
		// Construct Modular Synthesizer

		sample1.LoadAudioWaveform("./assets/SampleB.wav");

		// Primary oscillator
		osc1.waveform = olc::sound::synth::modules::Oscillator::Type::Wave;
		
		osc1.pWave = &sample1;
		
		osc1.frequency = 5.0 / 20000.0;
		osc1.amplitude = 0.5;
		osc1.parameter = 0.5;
		
		// Secondary one acts as tremelo for primary
		osc2.frequency = 10.0 / 20000.0;
		osc2.amplitude = 0.5;

		// Third oscillator acts as LFO for primary
		osc3.frequency = 5.0 / 20000.0;
		osc3.amplitude = 0.0;

		synth.AddModule(&osc1);
		synth.AddModule(&osc2);
		synth.AddModule(&osc3);
		synth.AddPatch(&osc2.output, &osc1.amplitude);
		synth.AddPatch(&osc3.output, &osc1.lfo_input);
		

		engine.InitialiseAudio();

		engine.SetCallBack_NewSample([this](double dTime) {return Synthesizer_OnNewCycleRequest(dTime); });
		engine.SetCallBack_SynthFunction([this](uint32_t nChannel, double dTime) {return Synthesizer_OnGetSample(nChannel, dTime); });
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::Q).bHeld)
			osc1.frequency.value *= 1.0 + 1.0 * fElapsedTime;

		if (GetKey(olc::Key::A).bHeld)
			osc1.frequency.value *= 1.0 - 1.0 * fElapsedTime;

		if (GetKey(olc::Key::W).bHeld)
			osc2.frequency.value *= 1.0 + 1.0 * fElapsedTime;

		if (GetKey(olc::Key::S).bHeld)
			osc2.frequency.value *= 1.0 - 1.0 * fElapsedTime;
		
		if (GetKey(olc::Key::E).bHeld)
			osc3.frequency.value += +0.01 * fElapsedTime;

		if (GetKey(olc::Key::D).bHeld)
			osc3.frequency.value += -0.01 * fElapsedTime;


		Clear(olc::BLACK);
		DrawString({ 4,  4 }, "OSC1 Frequency [+Q,-A]: " + std::to_string(osc1.frequency.value * 20000.0));
		DrawString({ 4, 14 }, "OSC2 Frequency [+W,-S]: " + std::to_string(osc2.frequency.value * 20000.0));
		DrawString({ 4, 24 }, "OSC3 Frequency [+E,-D]: " + std::to_string(osc3.frequency.value * 20000.0));
		return true;
	}
};



int main()
{
	BasicSynthesizer demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}