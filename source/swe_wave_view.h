#pragma once

#include "swe_prefix.h"

namespace olc::sound::wave
{
	///[OLC_HM] START WAVE_VIEW_TEMPLATE
	template<typename T>
	class View
	{
	public:
		View() = default;

		View(const T* pData, const size_t nSamples)
		{
			SetData(pData, nSamples);
		}

	public:
		void SetData(T const* pData, const size_t nSamples, const size_t nStride = 1, const size_t nOffset = 0)
		{
			m_pData = pData;
			m_nSamples = nSamples;
			m_nStride = nStride;
			m_nOffset = nOffset;
		}

		double GetSample(const double dSample) const
		{
			double d1 = std::floor(dSample);
			size_t p1 = static_cast<size_t>(d1);
			size_t p2 = p1 + 1;

			double t = dSample - d1;
			double a = GetValue(p1);
			double b = GetValue(p2);

			return a + t * (b - a); // std::lerp in C++20
		}

		std::pair<double, double> GetRange(const double dSample1, const double dSample2) const
		{
			if (dSample1 < 0 || dSample2 < 0)
				return { 0,0 };

			if (dSample1 > m_nSamples && dSample2 > m_nSamples)
				return { 0,0 };

			double dMin, dMax;

			double d = GetSample(dSample1);
			dMin = dMax = d;

			size_t n1 = static_cast<size_t>(std::ceil(dSample1));
			size_t n2 = static_cast<size_t>(std::floor(dSample2));
			for (size_t n = n1; n < n2; n++)
			{
				d = GetValue(n);
				dMin = std::min(dMin, d);
				dMax = std::max(dMax, d);
			}

			d = GetSample(dSample2);
			dMin = std::min(dMin, d);
			dMax = std::max(dMax, d);

			return { dMin, dMax };
		}

		T GetValue(const size_t nSample) const
		{
			if (nSample >= m_nSamples)
				return 0;
			else
				return m_pData[m_nOffset + nSample * m_nStride];
		}

	private:
		const T* m_pData = nullptr;
		size_t m_nSamples = 0;
		size_t m_nStride = 1;
		size_t m_nOffset = 0;
	};
	///[OLC_HM] END WAVE_VIEW_TEMPLATE
}
