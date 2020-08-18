#pragma once
#include <vector>
#include <numeric>

#include "RenderEngine.h"

class Statistic
{
public:

	float m_timeCounter;
	float m_lastTime;
	float m_start;

	int32_t m_maxUnits;
	int32_t m_maxTime;

	//first - time point; second - value
	std::vector<std::pair<int32_t, int32_t>> m_red_units;
	std::vector<std::pair<int32_t, int32_t>> m_blue_units;

	Statistic() 
	{
		m_maxUnits = 0;
		m_maxTime = 0;
		m_lastTime = 0.0f;
		m_timeCounter = 0.0f;

		m_red_units.push_back({ 0, 0 });
		m_blue_units.push_back({ 0, 0 });

		m_start = RenderEngine::getCurrTime();
	}

	void saveStatisticTimePoint(float delta, const std::vector<std::shared_ptr<Object>>& red_units,
											 const std::vector<std::shared_ptr<Object>>& blue_units)
	{
		m_timeCounter += delta;

		if (m_timeCounter - (m_lastTime /*- m_start*/) > 1.0f)
		{
			m_lastTime = m_timeCounter;

			m_red_units.push_back(std::pair<int32_t, int32_t>(int32_t(m_lastTime), int32_t(red_units.size())));
			m_blue_units.push_back(std::pair<int32_t, int32_t>(int32_t(m_lastTime), int32_t(blue_units.size())));
		}
	}

	std::vector<std::pair<float, float>> compressStatistic(const std::vector<std::pair<int32_t, int32_t>>& timeLine)
	{
		uint32_t koef = timeLine.size() / 250 + 1;

		std::vector<std::pair<float, float>> temp;

		for (uint32_t i = 0; i < timeLine.size(); i++)
		{
			float sumTime = 0;
			float sumUnits = 0;

			float j = 0;
			for (; j < koef && i < m_red_units.size(); i++, j++)
			{
				sumTime += timeLine[i].first;
				sumUnits += timeLine[i].second;
			}

			temp.push_back({(sumTime / j), (sumUnits / j)});
		}

		return temp;
	}

	void clear()
	{
		m_red_units.clear();
		m_blue_units.clear();
	}

	void resetLastTime()
	{
		m_lastTime = 0.0f;
	}

	float getMaxTime()
	{
		m_maxTime = m_red_units[m_red_units.size() - 1].first;
		return (float)m_maxTime;
	}

	float getMaxUnit()
	{
		auto pred = [&](std::pair<int32_t, int32_t>& element)
		{
			if (element.second > m_maxUnits)
				m_maxUnits = element.second;
			return false;
		};

		std::find_if(m_red_units.begin(), m_red_units.end(), pred);
		std::find_if(m_blue_units.begin(), m_blue_units.end(), pred);

		return (float)m_maxUnits;
	}
};