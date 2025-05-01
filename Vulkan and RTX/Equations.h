#pragma once

enum class EquationDifficulty : uint32_t
{
	EASY = 0,
	MEDIUM = 1,
	HARD = 2,
	INSANE = 3,
	BEYOND = 4,
	UNKNOWN = std::numeric_limits<uint32_t>::max(),
};

struct Equation
{
	std::string expression;
	double difficulty;
	int32_t damage;
	double answer; // exact integer until ~9 quadrillion
	double wrongAnswerPenalty;
	double timeToSolve;
};

namespace Equations
{
	extern std::vector<float> linearWeights;

	std::vector<Equation> generateEquations(size_t amount, double difficultyScale);
	std::string generate_positive_int_linear_equation(double difficulty);
	void printEquations(size_t amount, double difficultyScale);

	double solveForX(
		const std::string& equation, 
		double minX = -1000.0, double maxX = 1000.0, 
		double tolerance = 1e-3, 
		size_t maxIterations = 100
	);
}

