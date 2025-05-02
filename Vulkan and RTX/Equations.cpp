#include "pch.h"
#include "Equations.h"

#include "exprtk.hpp"

std::vector<float> Equations::positive_linear_weights = { 0.1196f, 0.1739f, 0.2860f, 0.4206f };
std::vector<float> Equations::negative_linear_weights = { 0.1237f, 0.1727f, 0.2924f, 0.4112f };

std::vector<Equation> Equations::generateEquations(size_t amount, double difficultyScale) {
	std::vector<Equation> equations;

	while (equations.size() < amount) {
		size_t equationType = randomInt(0, 1);
		double real_difficulty = 0;
		std::string equationString;
		//double real_difficulty = randomReal(0, 6) * difficultyScale;
		//int32_t int_difficulty = static_cast<int32_t>(real_difficulty);
		//double step = 6.0 * difficultyScale / static_cast<double>(amount);
		//real_difficulty = step * static_cast<double>(equations.size() + 1);
		switch (equationType) {
		case 0: 
			real_difficulty = randomNormalizedWeightedReal(positive_linear_weights) * 6 * difficultyScale;
			equationString = generate_positive_int_linear_equation(real_difficulty);
			break;
		case 1: 
			real_difficulty = randomNormalizedWeightedReal(negative_linear_weights) * 6 * difficultyScale;
			equationString = generate_negative_int_linear_equation(real_difficulty); 
			break;
		}
		if (equationString.empty()) continue;

		int32_t damage = std::max(static_cast<int32_t>(real_difficulty * 1.75), 1);
		//int32_t defence = static_cast<int32_t>(std::max(std::exp(real_difficulty * 0.33) - 1, 0.0));
		int32_t defence = static_cast<int32_t>(std::max(
			0.122 * real_difficulty * real_difficulty +
			0.307 * real_difficulty + 0.2,
			0.0));
		
		double rawPenalty = std::max(
			0.025 * real_difficulty * real_difficulty +
			0.130 * real_difficulty + 2.4, 0.0);
		double wrongAnswerPenalty = std::round(rawPenalty * 4.0) / 4.0;
		double timeToSolve = 9.5;
		double answer = solveForX(equationString);
		bool isSolved = false;

		//double answer = static_cast<double>(x);
		//std::cout << "real_difficulty: " << real_difficulty << "\n";
		//std::cout << "answer " << equations.size() << ": " << answer << "\n";
		//std::cout << "answer " << equations.size() << ": " << answer << "\n";

		if (!std::isnan(answer)) {
			equations.push_back({ 
				equationString, 
				real_difficulty, 
				damage, defence, 
				answer, 
				wrongAnswerPenalty, timeToSolve,
				isSolved
			});
		}
	}

	return equations;
}

std::string Equations::generate_positive_int_linear_equation(double difficulty)
{
	std::ostringstream oss;

	int32_t x = randomInt(
		1 + static_cast<int32_t>(difficulty * 0.9), 
		5 + static_cast<int32_t>(difficulty * 1.35)
	);
	int32_t a = randomInt(
		2 + static_cast<int32_t>(difficulty * 0.8),
		4 + static_cast<int32_t>(difficulty * 2.2)
	);
	int32_t b = randomInt(
		-5 - static_cast<int32_t>(difficulty * 2.0),
		5 + static_cast<int32_t>(difficulty * 6.0)
	);
	int32_t c = x * a + b;

	oss << a << "x + " << b << " = " << c;

	if (isLinearAcceptable(x, a, b, c, difficulty)) {
		return oss.str();
	}
	else {
		/*std::cout <<
			"equation is invalid, difficulty: " << difficulty << " " <<
			a << "x + " << b << " = " << c << "\n";*/
		return "";
	}
}
std::string Equations::generate_negative_int_linear_equation(double difficulty)
{
	std::ostringstream oss;

	int32_t x = -randomInt(
		1 + static_cast<int32_t>(difficulty * 0.9),
		5 + static_cast<int32_t>(difficulty * 1.35)
	);

	int32_t a = -randomInt(
		2 + static_cast<int32_t>(difficulty * 0.8),
		4 + static_cast<int32_t>(difficulty * 2.2)
	); // Negative 'a'

	int32_t b = -randomInt(
		-5 - static_cast<int32_t>(difficulty * 2.0),
		5 + static_cast<int32_t>(difficulty * 6.0)
	); // Mostly negative 'b'

	int32_t c = a * x + b;

	oss << a << "x + " << b << " = " << c;

	if (isLinearAcceptable(x, a, b, c, difficulty)) {
		return oss.str();
	}
	else {
		return "";
	}
}
bool Equations::isLinearAcceptable(int32_t x, int32_t a, int32_t b, int32_t c, double difficulty)
{
	if (difficulty >= 1.5) {
		if (a == b || a == x || b == x ||
			std::abs(x) < 2 || std::abs(a) < 2 || std::abs(b) < 2) {
			return false;
		}
	}
	if (difficulty >= 3.0) {
		if (a % 10 == 0 || b % 10 == 0 || x % 10 == 0 || 
			std::abs(x) < 5 || std::abs(a) < 4 || std::abs(b) < 7) {
			return false;
		}
	}
	if (difficulty >= 4.5) {
		if (a % 5 == 0 || b % 5 == 0 || x % 5 == 0 || 
			std::abs(x) < 7 || std::abs(a) < 6 || std::abs(b) < 11) {
			return false;
		}
	}

	return true;
}

void Equations::printEquations(size_t amount, double difficultyScale)
{
	std::vector<Equation> equations = generateEquations(amount, difficultyScale);

	size_t count_0_0___1_5 = 0;
	size_t count_1_5___3_0 = 0;
	size_t count_3_0___4_5 = 0;
	size_t count_4_5___6_0 = 0;
	size_t count_6_0___inf = 0;

	auto fraction = [amount](size_t count) {
		return static_cast<float>(count) / amount;
		};

	std::sort(equations.begin(), equations.end(), [](const Equation& a, const Equation& b) {
		return a.difficulty > b.difficulty;
	});

	for (const Equation& equation : equations) {
			 if (equation.difficulty < 1.5)	count_0_0___1_5++;
		else if (equation.difficulty < 3.0) count_1_5___3_0++;
		else if (equation.difficulty < 4.5) count_3_0___4_5++;
		else if (equation.difficulty < 6.0) count_4_5___6_0++;
									   else count_6_0___inf++;
		if (equation.difficulty >= 0.0) {
			std::cout
				<< "diff: " << std::fixed << std::setprecision(2) << equation.difficulty << ", "
				<< "dmg: " << equation.damage << ", "
				<< "def: " << equation.defence << ", "
				<< "expr: " << equation.expression << ", "
				<< "ans: " << std::fixed << std::setprecision(2) << equation.answer << ", "
				<< "tts: " << std::fixed << std::setprecision(2) << equation.wrongAnswerPenalty << "s"
				<< "\n";
		}
	}

	std::cout << "\nEquation difficulty distribution:\n";
	std::cout << "[0, 1.5):  " << count_0_0___1_5 << " (" << std::fixed << std::setprecision(4) << fraction(count_0_0___1_5) << "%)\n";
	std::cout << "[1.5, 3):  " << count_1_5___3_0 << " (" << fraction(count_1_5___3_0) << "%)\n";
	std::cout << "[3, 4.5):  " << count_3_0___4_5 << " (" << fraction(count_3_0___4_5) << "%)\n";
	std::cout << "[4.5, 6):  " << count_4_5___6_0 << " (" << fraction(count_4_5___6_0) << "%)\n";
	std::cout << "[6, +inf): " << count_6_0___inf << " (" << fraction(count_6_0___inf) << "%)\n";
	
	std::vector<float> fractions = {
		fraction(count_0_0___1_5),
		fraction(count_1_5___3_0),
		fraction(count_3_0___4_5),
		fraction(count_4_5___6_0),
		fraction(count_6_0___inf)
	};

	std::vector<float> invWeights;

	// Compute inverse weights, skipping zero fractions
	for (float& f : fractions) {
		if (f > 0.0f)
			invWeights.push_back(1.0f / f);
		else
			invWeights.push_back(0.0f);
	}

	// Normalize
	float invSum = std::accumulate(invWeights.begin(), invWeights.end(), 0.0);
	for (float& w : invWeights) {
		w /= invSum;
	}
	
	std::cout << "Normalized inverse weights: ";
	for (float w : invWeights) {
		std::cout << std::fixed << std::setprecision(4) << w << "f, ";
	}
	std::cout << "\n";
}

double Equations::solveForX(
	const std::string& equation,
	double minX, double maxX,
	double tolerance,
	size_t maxIterations
)
{
	using symbol_table_t = exprtk::symbol_table<double>;
	using expression_t = exprtk::expression<double>;
	using parser_t = exprtk::parser<double>;

	double x = 0.0;
	symbol_table_t symbol_table;
	symbol_table.add_variable("x", x);
	symbol_table.add_constants();

	expression_t expression;
	expression.register_symbol_table(symbol_table);

	// Convert equation "3x + 4 = 19" -> "3*x + 4 - 19"
	std::string lhs, rhs;
	size_t eq_pos = equation.find('=');
	if (eq_pos == std::string::npos) return std::numeric_limits<double>::quiet_NaN();
	lhs = equation.substr(0, eq_pos);
	rhs = equation.substr(eq_pos + 1);

	std::string expr = "(" + lhs + ")-(" + rhs + ")";

	parser_t parser;
	if (!parser.compile(expr, expression)) {
		std::cerr << "Failed to parse expression: " << expr << "\n";
		return std::numeric_limits<double>::quiet_NaN();
	}

	// Bisection method
	double a = minX, b = maxX;
	x = a; double fa = expression.value();
	x = b; double fb = expression.value();

	if (fa * fb > 0) {
		std::cerr << "No root in range [" << a << ", " << b << "]\n";
		return std::numeric_limits<double>::quiet_NaN();
	}

	for (size_t i = 0; i < maxIterations; ++i) {
		x = 0.5 * (a + b);
		double fx = expression.value();
		if (std::abs(fx) < tolerance) return x;

		x = a; fa = expression.value();
		if (fa * fx < 0)
			b = x = 0.5 * (a + b);
		else
			a = x = 0.5 * (a + b);
	}

	return x;
}
