#ifndef _CORE_UTILITIES_RANDOMGENERATOR_H_
#define _CORE_UTILITIES_RANDOMGENERATOR_H_

namespace rng
{
	// Random Ints
	inline int GetRandomInt(); //Shouldn't give negative numbers
	inline int GetRandomInt_InRange(int range_min, int range_max);

	// Vectors of Random Ints
	std::vector<int>GetRandomIntVec(int size);
	std::vector<int>GetRandomIntVec_InRange(int size, int range_min, int range_max);

	// Random Floats
	inline float GetRandomFloat();
	inline float GetRandomFloat_InRange(int range_min, int range_max);

	// Vectors of Random Floats
	std::vector<float>GetRandomFloatVec(int size);
	std::vector<float>GetRandomFloatVec_InRange(int size, float range_min, float range_max);

} //namespace rng

#endif //CORE_UTILITIES_RANDOMGENERATOR_H