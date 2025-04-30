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
	int32_t difficulty;
	int32_t damage;
	double answer; // exact until ~9 quadrillion
	double timeToSolve;
};

namespace Equations
{
	std::vector<Equation> generateEquations();
}

