#include "pch.h"
#include "Equations.h"

std::vector<Equation> Equations::generateEquations(size_t amount, float difficultyScale) {
	std::vector<Equation> equations;

	for (size_t i = 0; i < amount; ++i) {
		int32_t difficulty = static_cast<int32_t>(randomInt(0, 4) * difficultyScale);
		int32_t damage = std::max(difficulty * 2, 1);
		int32_t x = randomInt(1 + difficulty, 7 + difficulty);
		int32_t a = randomInt(1 + difficulty, 7 + difficulty);
		int32_t b = x * a + randomInt(-5 - difficulty, 5 + difficulty);

		std::ostringstream oss;
		oss << a << "x + " << (b - a * x) << " = " << b;

		double answer = static_cast<double>(x);
		double timeToSolve = 11.0;
		std::cout << "answer" << i << " " << answer << "\n";

		equations.push_back({ oss.str(), difficulty, damage, answer, timeToSolve });
	}

	return equations;
}