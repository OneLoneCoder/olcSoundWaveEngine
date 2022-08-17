#include "swe_synth_modular.h"





namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_MODULAR_CPP
	Property::Property(double f)
	{
		value = std::clamp(f, -1.0, 1.0);
	}

	Property& Property::operator =(const double f)
	{
		value = std::clamp(f, -1.0, 1.0);
		return *this;
	}


	ModularSynth::ModularSynth()
	{

	}

	bool ModularSynth::AddModule(Module* pModule)
	{
		// Check if module already added
		if (std::find(m_vModules.begin(), m_vModules.end(), pModule) == std::end(m_vModules))
		{
			m_vModules.push_back(pModule);
			return true;
		}

		return false;
	}

	bool ModularSynth::RemoveModule(Module* pModule)
	{
		if (std::find(m_vModules.begin(), m_vModules.end(), pModule) == std::end(m_vModules))
		{
			m_vModules.erase(std::remove(m_vModules.begin(), m_vModules.end(), pModule), m_vModules.end());
			return true;
		}

		return false;
	}

	bool ModularSynth::AddPatch(Property* pInput, Property* pOutput)
	{
		// Does patch exist?
		std::pair<Property*, Property*> newPatch = std::pair<Property*, Property*>(pInput, pOutput);

		if (std::find(m_vPatches.begin(), m_vPatches.end(), newPatch) == std::end(m_vPatches))
		{
			// Patch doesnt exist, now check if either are null
			if (pInput != nullptr && pOutput != nullptr)
			{
				m_vPatches.push_back(newPatch);
				return true;
			}
		}

		return false;		
	}

	bool ModularSynth::RemovePatch(Property* pInput, Property* pOutput)
	{
		std::pair<Property*, Property*> newPatch = std::pair<Property*, Property*>(pInput, pOutput);

		if (std::find(m_vPatches.begin(), m_vPatches.end(), newPatch) == std::end(m_vPatches))
		{
			m_vPatches.erase(std::remove(m_vPatches.begin(), m_vPatches.end(), newPatch), m_vPatches.end());
			return true;
		}

		return false;
	}

	void ModularSynth::UpdatePatches()
	{
		// Update patches
		for (auto& patch : m_vPatches)
		{
			patch.second->value = patch.first->value;
		}
	}


	void ModularSynth::Update(uint32_t nChannel, double dTime, double dTimeStep)
	{
		// Now update synth
		for (auto& pModule : m_vModules)
		{
			pModule->Update(nChannel, dTime, dTimeStep);
		}
	}

	///[OLC_HM] END SYNTH_MODULAR_CPP
}

