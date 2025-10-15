#include "pch.h"
#include "Market.h"

void Market::simulateMarket(size_t tradersCount, size_t months)
{
	// set traders data
	const double startWealth = 100.0;
	std::vector<Trader> traders(tradersCount);
	for (auto& t : traders) {
		t.wealth = startWealth;
		t.skill = randomReal(-0.005, 0.005);
	}
	
	const double redistribution = 0.01;
	for (size_t m = 0; m < months; ++m) {
		simulateOrderBook(traders, 1);

		// redistribute wealth
		double totalWealth = 0.0;
		for (auto& t : traders) totalWealth += t.wealth;
		double meanWealth = totalWealth / tradersCount;
		for (auto& t : traders) {
			t.wealth = (1.0 - redistribution) * t.wealth + redistribution * meanWealth;
		}
	}

	std::sort(traders.begin(), traders.end(), 
		[](Trader& a, Trader& b) { return a.wealth > b.wealth; }
	);

	findMarketStats(traders, months);
}

void Market::simulateOrderBook(std::vector<Trader>& traders, size_t steps)
{
	std::vector<MarketOrder> marketOrders;
	marketOrders.reserve(traders.size());

	const double sigma = 0.05; // volatility
	std::normal_distribution<double> dist(0.0, sigma);

	// traders submit random buy/sell orders
	for (size_t i = 0; i < traders.size(); ++i) {
		bool isBuy = randomInt(0, 1);
		double basePrice = 100.0;
		double price = basePrice * (1.0 + dist(generator_32) * 0.1);  // ±10% fluctuation
		double quantity = std::min(traders[i].wealth / basePrice, static_cast<double>(randomReal(0.1, 1.0)));
		marketOrders.push_back({ i, price, quantity, isBuy });
	}

	// sort order book
	std::vector<MarketOrder> bids, asks;
	for (auto& o : marketOrders) {
		(o.isBuy ? bids : asks).push_back(o);
	}

	std::sort(bids.begin(), bids.end(), [](auto& a, auto& b) { return a.price > b.price; });
	std::sort(asks.begin(), asks.end(), [](auto& a, auto& b) { return a.price < b.price; });

	// match orders
	size_t i = 0, j = 0;
	while (i < bids.size() && j < asks.size()) {
		if (bids[i].price >= asks[j].price) {
			double tradePrice = 0.5 * (bids[i].price + asks[j].price);
			double tradeQty = std::min(bids[i].quantity, asks[j].quantity);
			double tradeValue = tradePrice * tradeQty;

			// friction (e.g., transaction fee)
			//double fee = tradeValue * 0.001;
			double fee = 0.0;

			traders[bids[i].traderId].wealth -= tradeValue + fee;
			traders[asks[j].traderId].wealth += tradeValue - fee;

			bids[i].quantity -= tradeQty;
			asks[j].quantity -= tradeQty;

			if (bids[i].quantity <= 1e-5) ++i;
			if (asks[j].quantity <= 1e-5) ++j;
		}
		else break;
	}
}

void Market::findMarketStats(std::vector<Trader>& traders, size_t months)
{
	size_t tradersCount = traders.size();

	auto sumWealth = [](auto begin, auto end) {
		return std::accumulate(
			begin, end, 0.0,
			[](double sum, const Trader& t) { return sum + t.wealth; }
		);
	};

	size_t top1count = std::max<size_t>(1, tradersCount / 100);
	size_t top10count = std::max<size_t>(1, tradersCount / 10);

	double total = sumWealth(traders.begin(), traders.end());
	double mean = total / tradersCount;
	double median = (tradersCount % 2 == 0)
		? (traders[tradersCount / 2 - 1].wealth + traders[tradersCount / 2].wealth) / 2.0
		: traders[tradersCount / 2].wealth;
	double top1 = sumWealth(traders.begin(), traders.begin() + top1count);
	double top10 = sumWealth(traders.begin(), traders.begin() + top10count);
	double bottom50 = sumWealth(traders.begin() + tradersCount / 2, traders.end());

	std::cout << "\n---Zero-Sum Market Simulation---\n";
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "Traders: " << tradersCount << ", Months: " << months << "\n";
	std::cout << "Total wealth: " << total << "\n";
	std::cout << "Mean: " << mean << ", Median: " << median << "\n";
	std::cout << "Best: " << traders.front().wealth << ", Worst: " << traders.back().wealth << "\n";
	std::cout << "Top 1% share: " << (top1 / total * 100.0) << "%\n";
	std::cout << "Top 10% share: " << (top10 / total * 100.0) << "%\n";
	std::cout << "Bottom 50% share: " << (bottom50 / total * 100.0) << "%\n\n";

	std::cout << "Wealth by percentile:\n";
	const size_t step = 5; // print every N%
	for (size_t p = 0; p <= 100; p += step) {
		size_t idx = std::clamp<size_t>(
			static_cast<size_t>(p / 100.0 * (tradersCount - 1)),
			0, tradersCount - 1
		);
		std::cout << std::setw(3) << 100 - p << "% : " << std::fixed << std::setprecision(3)
			<< traders[idx].wealth << "\n";
	}
}
