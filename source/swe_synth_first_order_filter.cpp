#pragma once
#include "swe_synth_first_order_filter.h"


namespace olc::sound::synth
{
	///[OLC_HM] START SYNTH_FIRST_ORDER_FILTER_CPP
	namespace modules
	{
		void FirstOrderFilter::Update(uint32_t nChannel, double dTime, double dTimeStep)
		{
			double new_state = input.value + pole.value * state.value;
			output = new_state - zero.value * state.value;
			state = new_state;
		}
	}
	///[OLC_HM] END SYNTH_FIRST_ORDER_FILTER_CPP
}