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
	const double sigma = 0.05; // volatility
	std::normal_distribution<double> dist(0.0, sigma);

	// clear previous orders
	for (auto& t : traders) {
		t.orders.clear();
	}

	// traders submit random buy/sell orders
	for (auto& t : traders) {
		bool isBuy = randomInt(0, 1);
		double basePrice = 100.0;
		double price = basePrice * (1.0 + dist(generator_32) * 0.1);  // ±10% fluctuation
		double quantity = std::min(t.wealth / basePrice, static_cast<double>(randomReal(0.1, 1.0)));
		t.orders.push_back({ price, quantity, isBuy });
	}

	// sort order book
	std::vector<std::pair<size_t, MarketOrder>> bids, asks;
	for (size_t i = 0; i < traders.size(); ++i) {
		for (auto& o : traders[i].orders) {
			(o.isBuy ? bids : asks).emplace_back(i, o);
		}
	}

	std::sort(bids.begin(), bids.end(), [](auto& a, auto& b) { return a.second.price > b.second.price; });
	std::sort(asks.begin(), asks.end(), [](auto& a, auto& b) { return a.second.price < b.second.price; });

	// match orders
	size_t i = 0, j = 0;
	while (i < bids.size() && j < asks.size()) {
		MarketOrder& bidOrder = bids[i].second;
		MarketOrder& askOrder = asks[j].second;
		if (bidOrder.price >= askOrder.price) {
			double tradePrice = 0.5 * (bidOrder.price + askOrder.price);
			double tradeQty = std::min(bidOrder.quantity, askOrder.quantity);
			double tradeValue = tradePrice * tradeQty;

			// friction (e.g., transaction fee)
			//double fee = tradeValue * 0.001;
			double fee = 0.0;

			traders[bids[i].first].wealth -= tradeValue + fee;
			traders[asks[j].first].wealth += tradeValue - fee;

			bidOrder.quantity -= tradeQty;
			askOrder.quantity -= tradeQty;

			if (bidOrder.quantity <= 1e-5) {
				bidOrder.isMatched = true;
				++i;
			} 
			if (askOrder.quantity <= 1e-5) {
				askOrder.isMatched = true;
				++j;
			}
		}
		else break;
	}

	getOrderBook(bids, asks);
	
	//for (auto& t : traders) {
	//	t.orders.erase(
	//		std::remove_if(t.orders.begin(), t.orders.end(),
	//			[](const MarketOrder& o) { return o.isMatched; }),
	//		t.orders.end()
	//	);
	//}
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

void Market::getOrderBook(Orders bids, Orders asks)
{
	const size_t N = 30;
	std::cout << std::fixed << std::setprecision(3);

	// Sort bids descending, asks ascending
	std::sort(bids.begin(), bids.end(), [](auto& a, auto& b) { return a.second.price > b.second.price; });
	std::sort(asks.begin(), asks.end(), [](auto& a, auto& b) { return a.second.price > b.second.price; });

	Orders topAsks;

	// collect last N unmatched asks
	for (auto it = asks.rbegin(); it != asks.rend() && topAsks.size() < N; ++it) {
		if (!it->second.isMatched) {
			topAsks.push_back(*it);
		}
	}

	// print them in descending order (highest price first)
	std::reverse(topAsks.begin(), topAsks.end());

	for (auto& entry : topAsks) {
		const auto& order = entry.second;
		std::cout << "ASK | Trader " << std::setw(3) << entry.first
			<< " | Price: " << std::setw(8) << order.price
			<< " | Qty: " << std::setw(8) << order.quantity << "\n";
	}

	// Print bids first (highest price at top)
	size_t count = 0;
	for (auto& entry : bids) {
		const auto& order = entry.second;
		if (!order.isMatched) {
			std::cout << "BID | Trader " << std::setw(3) << entry.first
				<< " | Price: " << std::setw(8) << order.price
				<< " | Qty: " << std::setw(8) << order.quantity << "\n";
			if (++count >= N) break;
		}
	}

	std::cout << "\n";
}
