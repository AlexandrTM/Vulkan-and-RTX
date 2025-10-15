#include "pch.h"
#include "Equations.h"

#include "exprtk.hpp"

std::vector<float> Equations::positive_linear_weights = { 0.0949f, 0.1369f, 0.3028f, 0.4653f };
std::vector<float> Equations::negative_linear_weights = { 0.0928f, 0.1411f, 0.3036f, 0.4625f };

std::vector<Equation> Equations::generateEquations(size_t amount, double difficultyScale) {
	std::vector<Equation> equations;

	while (equations.size() < amount) {
		size_t equationType = randomInt(0, 1);
		double real_difficulty = 0;
		std::string equationString;
		//real_difficulty = randomReal(0, 6) * difficultyScale;

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
			equationString = generate_negative_int_linear_equation(real_difficulty * 0.93); 
			break;
		}
		if (equationString.empty()) continue;

		int32_t damage = 0;
		int32_t defence = 0;

		damage = 
			static_cast<int32_t>(std::max(real_difficulty * 1.75 * randomReal(0.85f, 1.15f), 1.0));
		//defence = static_cast<int32_t>(std::max(std::exp(real_difficulty * 0.33) - 1, 0.0));
		//defence = static_cast<int32_t>(std::min(std::max(
		//	0.122 * real_difficulty * real_difficulty +
		//	0.307 * real_difficulty/* + 0.2*/,
		//	0.0), 7.0));
		defence = randomReal(0.0f + real_difficulty * 0.25, std::max(real_difficulty * 1.1, 1.25));
		
		double rawAnswerPenalty = std::max(
			0.025 * real_difficulty * real_difficulty +
			0.130 * real_difficulty + 2.2, 0.0);
		double wrongAnswerPenalty = std::round(rawAnswerPenalty * 4.0) / 4.0;
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
		if (std::abs(a) == std::abs(b) || std::abs(a) == std::abs(x) || std::abs(b) == std::abs(x) ||
			std::abs(x) < 2 || std::abs(a) < 2 || std::abs(b) < 2) {
			return false;
		}
	}
	if (difficulty >= 3.0) {
		if (std::abs(a) % 10 == 0 || std::abs(b) % 10 == 0 || std::abs(x) % 10 == 0 ||
			std::abs(x) < 5 || std::abs(a) < 4 || std::abs(b) < 7) {
			return false;
		}
	}
	if (difficulty >= 4.5) {
		if (std::abs(a) % 5 == 0 || std::abs(b) % 5 == 0 || std::abs(x) % 5 == 0 ||
			std::abs(x) < 7 || std::abs(a) < 6 || std::abs(b) < 11) {
			return false;
		}
	}
	if (difficulty >= 6.0) {
		if (std::abs(c) % std::abs(b) == 0) {
			return false;
		}
	}

	return true;
}

void Equations::debugEquations(size_t amount, double difficultyScale)
{
	std::vector<Equation> equations = generateEquations(amount, difficultyScale);

	std::vector<double> thresholds = { 0.0, 1.5, 3.0, 4.5, 6.0 }; // Start inclusive, last bin is [6.0, +inf)
	std::vector<size_t> binCounts(thresholds.size(), 0);

	double totalAnswer = 0.0;
	int32_t totalDamage = 0;
	int32_t totalDefence = 0;
	double totalPenalty = 0.0;
	double totalTimeToSolve = 0.0;

	auto fraction = [amount](size_t count) {
		return static_cast<float>(count) / amount;
		};

	std::sort(equations.begin(), equations.end(), [](const Equation& a, const Equation& b) {
		return a.difficulty > b.difficulty;
	});

	for (const Equation& equation : equations) {
		bool counted = false;
		for (size_t i = 1; i < thresholds.size(); ++i) { // start from second threshold
			if (equation.difficulty < thresholds[i]) {
				binCounts[i - 1]++;
				counted = true;
				break;
			}
		}
		if (!counted) {
			binCounts.back()++; // Last bin [thresholds.back(), +inf)
		}

		totalAnswer += std::abs(equation.answer);
		totalDamage += equation.damage;
		totalDefence += equation.defence;
		totalPenalty += equation.wrongAnswerPenalty;
		totalTimeToSolve += equation.timeToSolve;

		if (equation.difficulty >= 10.0) {
			std::cout
				<< "diff: " << std::fixed << std::setprecision(2) << equation.difficulty << ", "
				<< "dmg: "  << std::setw(3)  << std::left << equation.damage << ", "
				<< "def: "  << std::setw(3)   << equation.defence << ", "
				<< "expr: " << std::setw(20)  << equation.expression << ", "
				<< "ans: "  << std::setw(7)   << std::fixed << std::setprecision(2) << equation.answer << ", "
				<< "wap: "  << std::setw(5)  << std::fixed << std::setprecision(2) << equation.wrongAnswerPenalty << "s"
				<< "\n";
		}
	}

	std::vector<float> fractions;
	for (size_t count : binCounts) {
		fractions.push_back(fraction(count));
	}

	// Print difficulty distribution
	std::cout << "\nEquation difficulty distribution:\n" << std::fixed << std::setprecision(4);
	for (size_t i = 1; i < thresholds.size(); ++i) {
		std::cout << "[" << thresholds[i - 1] << ", " << thresholds[i] << "): "
			<< binCounts[i - 1] << " ("
			<< fractions[i - 1] << "%)\n";
	}
	std::cout << "[" << thresholds.back() << ", +inf): "
		<< binCounts.back() << " (" << fractions.back() << "%)\n";

	std::cout << "\n***Average stats***\n";
	std::cout << "answer:        " << std::fixed << std::setprecision(4) << totalAnswer / equations.size() << "\n";
	std::cout << "damage:        " << totalDamage / static_cast<double>(equations.size()) << "\n";
	std::cout << "defence:       " << totalDefence / static_cast<double>(equations.size()) << "\n";
	std::cout << "wrong penalty: " << totalPenalty / equations.size() << "s\n";
	std::cout << "time to solve: " << totalTimeToSolve / equations.size() << "s\n";

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

void Equations::simulateMarket(size_t traders, size_t months)
{
	const double startWealth = 1000.0;
	const double sigma = 0.05; // volatility

	std::vector<double> wealth(traders, startWealth);

	std::vector<double> skill(traders);
	for (auto& s : skill) {
		s = randomReal(-0.005, 0.005);
	}

	std::normal_distribution<double> dist(0.0, sigma);

	for (size_t m = 0; m < months; ++m) {
		std::vector<double> investmentsReturn(traders);
		for (size_t i = 0; i < traders; ++i) {
			//investmentsReturn[i] = randomReal(-sigma, sigma) * std::pow(wealth[i] / startWealth, 0.25) + skill[i];
			//investmentsReturn[i] = randomReal(-sigma, sigma) + skill[i];
			investmentsReturn[i] = dist(generator_32) + skill[i];
		}

		// zero-sum adjustment
		double totalWealth = std::accumulate(wealth.begin(), wealth.end(), 0.0);
		double weightedMean = 0.0;
		for (size_t i = 0; i < traders; ++i) {
			weightedMean += wealth[i] * investmentsReturn[i];
		}
		weightedMean /= totalWealth;

		for (size_t i = 0; i < traders; ++i) {
			double r = investmentsReturn[i] - weightedMean;
			if (r < -0.99) r = -0.99;
			wealth[i] *= (1.0 + r);
		}
	}

	std::sort(wealth.begin(), wealth.end(), std::greater<>());

	std::cout << "\nWealth by percentile:\n";
	const size_t step = 2; // print every N%
	for (size_t p = 0; p <= 100; p += step) {
		size_t idx = std::clamp<size_t>(
			static_cast<size_t>(p / 100.0 * (traders - 1)),
			0, traders - 1
		);
		std::cout << std::setw(3) << 100 - p << "% : " << std::fixed << std::setprecision(2)
			<< wealth[idx] << "\n";
	}

	int top1count = std::max<size_t>(1, traders / 100);
	int top10count = std::max<size_t>(1, traders / 10);

	double total = std::accumulate(wealth.begin(), wealth.end(), 0.0);
	double mean = total / traders;
	double median = (traders % 2 == 0)
		? (wealth[traders / 2 - 1] + wealth[traders / 2]) / 2.0
		: wealth[traders / 2];
	double top1 = std::accumulate(wealth.begin(), wealth.begin() + top1count, 0.0);
	double top10 = std::accumulate(wealth.begin(), wealth.begin() + top10count, 0.0);
	double bottom50 = std::accumulate(wealth.begin() + traders / 2, wealth.end(), 0.0);

	std::cout << std::fixed << std::setprecision(2);
	std::cout << "=== Zero-Sum Market Simulation ===\n";
	std::cout << "Traders: " << traders << ", Months: " << months << "\n";
	std::cout << "Total wealth: " << total << "\n";
	std::cout << "Mean: " << mean << ", Median: " << median << "\n";
	std::cout << "Top 1% share: " << (top1 / total * 100.0) << "%\n";
	std::cout << "Top 10% share: " << (top10 / total * 100.0) << "%\n";
	std::cout << "Bottom 50% share: " << (bottom50 / total * 100.0) << "%\n";
	std::cout << "Best: " << wealth.front() << ", Worst: " << wealth.back() << "\n\n";
}
