#include "UUID.h"

#include <random>
#include <unordered_map>

namespace ZenEngine
{

	static std::random_device sRandomDevice;
	static std::mt19937_64 sEngine(sRandomDevice());
	static std::uniform_int_distribution<uint64_t> sUniformDistribution;

	UUID::UUID()
		: mUUID(sUniformDistribution(sEngine))
	{
	}

	UUID::UUID(uint64_t inUUID)
		: mUUID(inUUID)
	{
	}

}