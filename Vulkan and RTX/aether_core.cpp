#include "pch.h"
#include "aether_core.h"

boost::random::mt19937 generator;
boost::random::mt19937_64 generator64;
std::default_random_engine gen;

void seedRandomGenerator() {
	std::random_device rd;  // Get a random seed from the system (if available)
	generator.seed(rd());  // Seed the boost RNG with the random device
	generator64.seed(rd());
	srand(static_cast<unsigned>(time(0))); // seed std generator
}

float randomReal(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}
float randomRealOffset(float offset) {
	return randomReal(-offset, offset);
}
float randomNormalizedReal() {
	boost::random::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	return distribution(generator);
}
uint64_t randomHash64() {
	boost::random::uniform_int_distribution<uint64_t> distribution;
	return distribution(generator64);
}

// Create weighted probability distribution for the intervals
std::vector<std::pair<float, float>> distributionIntervals = {
	{0.0, 0.25}, {0.25, 0.5}, {0.5, 0.75}, {0.75, 1.0}
};
float randomNormalizedWeightedReal(std::vector<float>& weights)
{
	// Select an interval based on the weighted distribution, right symbol is sum of weights
	float randWeight = randomReal(0, 1);
	float accumulatedWeight = 0.0;
	int selectedInterval = 0;

	for (size_t i = 0; i < weights.size(); ++i) {
		accumulatedWeight += weights[i];
		if (randWeight < accumulatedWeight) {
			selectedInterval = i;
			break;
		}
	}

	// Now select a real value within the selected interval
	float intervalStart = distributionIntervals[selectedInterval].first;
	float intervalEnd = distributionIntervals[selectedInterval].second;

	// Return the random real within the selected range
	return randomReal(intervalStart, intervalEnd);
}