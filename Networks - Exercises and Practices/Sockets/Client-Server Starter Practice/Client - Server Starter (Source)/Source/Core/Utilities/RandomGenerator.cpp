#include "RandomGenerator.h"

// ---- Construction ----
RandomGenerator::RandomGenerator()
{
	m_RNEngine = std::default_random_engine(m_RandomDevice());
}


// ---- Ints ----
inline int RandomGenerator::GetRandomInt() //Not working
{
	return m_IntDistribution(m_RNEngine);
}

inline int RandomGenerator::GetRandomInt_InRange(int range_min, int range_max)
{
	std::uniform_int_distribution<int> tmp_IntDistribution(range_min, range_max);
	return tmp_IntDistribution(m_RNEngine);
}


// ---- Ints Vec ----
std::vector<int> RandomGenerator::GetRandomIntVec(int size)
{
	std::uniform_int_distribution<int> tmp_IntDistribution(0, size - 1);
	std::vector<int> ret;
	
	for (int i = 0; i < size - 1; i++)
		ret.push_back(tmp_IntDistribution(m_RNEngine));
	
	return ret;
}

std::vector<int> RandomGenerator::GetRandomIntVec_InRange(int size, int range_min, int range_max)
{
	std::uniform_int_distribution<int> tmp_IntDistribution(range_min, range_max);
	std::vector<int> ret;
	
	for (int i = 0; i < size - 1; i++)
		ret.push_back(tmp_IntDistribution(m_RNEngine));
	
	return ret;
}


// ---- Floats ----
inline float RandomGenerator::GetRandomFloat()
{
	return (float)m_DoubleDistribution(m_RNEngine);
}

inline float RandomGenerator::GetRandomFloat_InRange(int range_min, int range_max)
{
	std::uniform_real_distribution<double> tmp_DoubleDistribution(range_min, range_max);
	return (float)tmp_DoubleDistribution(m_RNEngine);
}


// ---- Floats Vec ----
std::vector<float> RandomGenerator::GetRandomFloatVec(int size)
{
	std::uniform_real_distribution<double> tmp_DoubleDistribution;
	std::vector<float> ret;
	
	for (int i = 0; i < size - 1; i++)
		ret.push_back((float)tmp_DoubleDistribution(m_RNEngine));
	
	return ret;
}

std::vector<float> RandomGenerator::GetRandomFloatVec_InRange(int size, float range_min, float range_max)
{
	std::uniform_real_distribution<double> tmp_DoubleDistribution(range_min, range_max);
	std::vector<float> ret;
	
	for (int i = 0; i < size - 1; i++)
		ret.push_back((float)tmp_DoubleDistribution(m_RNEngine));
	
	return ret;
}