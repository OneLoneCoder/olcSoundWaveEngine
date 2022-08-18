#pragma once
#include "swe_prefix.h"


namespace olc::sound
{
	
	namespace synth
	{
		///[OLC_HM] START SYNTH_MODULAR_H
		class Property
		{
		public:
			double value = 0.0f;

		public:
			Property() = default;
			Property(double f);

		public:
			Property& operator =(const double f);			
		};


		class Trigger
		{

		};


		class Module
		{
		public:
			virtual void Update(uint32_t nChannel, double dTime, double dTimeStep) = 0;
		};


		class ModularSynth
		{
		public:
			ModularSynth();

		public:
			bool AddModule(Module* pModule);
			bool RemoveModule(Module* pModule);
			bool AddPatch(Property* pInput, Property* pOutput);
			bool RemovePatch(Property* pInput, Property* pOutput);


		public:
			void UpdatePatches();
			void Update(uint32_t nChannel, double dTime, double dTimeStep);

		protected:
			std::vector<Module*> m_vModules;
			std::vector<std::pair<Property*, Property*>> m_vPatches;
		};

		///[OLC_HM] END SYNTH_MODULAR_H
	}	
}