#include "pch.h"
#include "Equations.h"

std::vector<Equation> Equations::generateEquations() {
	std::vector<Equation> equations;

	for (size_t i = 0; i < 3; ++i) {
		int32_t difficulty = randomInt(0, 4);
		int32_t damage = std::max(difficulty * 2, 1);
		int32_t x = randomInt(1, 10);
		int32_t a = randomInt(1, 10);
		int32_t b = x * a + randomInt(-5, 5);

		std::ostringstream oss;
		oss << a << "x + " << (b - a * x) << " = " << b;

		equations.push_back({ oss.str(), difficulty, damage, 1 });
	}

	return equations;
}