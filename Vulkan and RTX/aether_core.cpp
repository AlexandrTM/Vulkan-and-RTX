#include "pch.h"
#include "aether_core.h"

boost::random::mt19937 generator;
std::default_random_engine gen;

void seedRandomGenerator() {
	std::random_device rd;  // Get a random seed from the system (if available)
	generator.seed(rd());  // Seed the boost RNG with the random device
	srand(static_cast<unsigned>(time(0))); // seed std generator
}

float randomReal(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}
float randomNormalizedReal() {
	boost::random::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	return distribution(generator);
}