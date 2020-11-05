#include <random>
#include <vector>

namespace rng
{

	std::random_device m_RandomDevice;
	std::default_random_engine m_RNEngine = std::default_random_engine(m_RandomDevice());

	std::uniform_real_distribution<double> m_DoubleDistribution;
	std::uniform_int_distribution<int> m_IntDistribution;


	// ---- Ints ----
	int GetRandomInt()
	{
		return m_IntDistribution(m_RNEngine);
	}

	int GetRandomInt_InRange(int range_min, int range_max)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution(range_min, range_max);
		return tmp_IntDistribution(m_RNEngine);
	}


	// ---- Ints Vec ----
	std::vector<int> GetRandomIntVec(int size)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution;
		std::vector<int> ret;

		for (int i = 0; i < size; i++)
			ret.push_back(tmp_IntDistribution(m_RNEngine));

		return ret;
	}

	std::vector<int> GetRandomIntVec_InRange(int size, int range_min, int range_max)
	{
		std::uniform_int_distribution<int> tmp_IntDistribution(range_min, range_max);
		std::vector<int> ret;

		for (int i = 0; i < size; i++)
			ret.push_back(tmp_IntDistribution(m_RNEngine));

		return ret;
	}


	// ---- Floats ----
	float GetRandomFloat()
	{
		return (float)m_DoubleDistribution(m_RNEngine);
	}

	float GetRandomFloat_InRange(int range_min, int range_max)
	{
		std::uniform_real_distribution<double> tmp_DoubleDistribution(range_min, range_max);
		return (float)tmp_DoubleDistribution(m_RNEngine);
	}


	// ---- Floats Vec ----
	std::vector<float> GetRandomFloatVec(int size)
	{
		std::uniform_real_distribution<double> tmp_DoubleDistribution;
		std::vector<float> ret;

		for (int i = 0; i < size; i++)
			ret.push_back((float)tmp_DoubleDistribution(m_RNEngine));

		return ret;
	}

	std::vector<float> GetRandomFloatVec_InRange(int size, float range_min, float range_max)
	{
		std::uniform_real_distribution<double> tmp_DoubleDistribution(range_min, range_max);
		std::vector<float> ret;

		for (int i = 0; i < size; i++)
			ret.push_back((float)tmp_DoubleDistribution(m_RNEngine));

		return ret;
	}

} //namespace rng