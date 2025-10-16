#include "pch.h"
#include "Market.h"

void Market::simulateMarket()
{
	//matplot::figure();
	//matplot::hold(true);

	std::vector<double> priceHistory;
	for (size_t i = 0; i < 1; ++i) {
		priceHistory = Market::simulateSecurity(5000, 1000);
	}

	//matplot::plot(priceHistory);
	//matplot::title("Price over Time");
	//matplot::xlabel("Month");
	//matplot::ylabel("Price");
	//matplot::show();
}

std::vector<double> Market::simulateSecurity(size_t tradersCount, size_t months)
{
	const double startWealth = 100.0;
	const double basePrice = 100.0;
	double currentPrice = 100.0;
	std::vector<double> priceHistory;

	const double startHoldings = startWealth / basePrice;

	std::vector<Trader> traders(tradersCount);
	for (auto& t : traders) {
		t.wealth = startWealth;
		t.skill = randomReal(-0.0025, 0.0025);
		t.positionLimit = randomReal(0.01, 1.0);
		t.holdings = startHoldings;
	}

	priceHistory.push_back(currentPrice);
	for (size_t m = 0; m < months; ++m) {
		simulateOrderBook(traders, basePrice, currentPrice, priceHistory);

		redistributeWealth(traders);

		double totalWealth = 0.0;
		for (auto& t : traders) totalWealth += t.wealth;
		double wealthScalingFactor = totalWealth / (startWealth * tradersCount);
		for (auto& t : traders) t.wealth /= wealthScalingFactor;
	}

	std::sort(traders.begin(), traders.end(), 
		[](Trader& a, Trader& b) { return a.wealth > b.wealth; }
	);

	findMarketStats(traders, months, priceHistory);

	return priceHistory;
}
void Market::simulateOrderBook(
	std::vector<Trader>& traders,
	const double& basePrice,
	double& currentPrice,
	std::vector<double>& priceHistory
)
{
	static const double sigma = 0.1; // volatility
	//static std::normal_distribution<double> dist(0.0, sigma);
	static std::normal_distribution<double> dist(0.0, sigma);

	// clear previous orders
	for (auto& t : traders) {
		t.orders.clear();
	}

	// traders submit random buy/sell orders
	for (auto& t : traders) {
		bool isBuy = randomInt(0, 1);
		//double priceFluctuation = std::tanh(dist(generator_32)) + 1;
		//double priceFluctuation = std::exp(dist(generator_32));
		double priceFluctuation = std::exp(dist(generator_32) - 0.5 * sigma * sigma);
		//double priceFluctuation = dist(generator_32) * basePrice;
		//std::cout << priceFluctuation << "\n";
		//double price = currentPrice + priceFluctuation * basePrice;
		double price = currentPrice * priceFluctuation;
		double maxAffordableQuantity = 0.0;
		if (isBuy)
			maxAffordableQuantity = t.wealth / price;
		else
			maxAffordableQuantity = t.holdings;
		//maxAffordableQuantity = t.wealth / price;
		//maxAffordableQuantity = t.wealth / currentPrice;
		double quantity = maxAffordableQuantity * randomReal(0.0, t.positionLimit);
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
	double totalValue = 0.0, totalQty = 0.0;
	while (i < bids.size() && j < asks.size()) {
		MarketOrder& bidOrder = bids[i].second;
		MarketOrder& askOrder = asks[j].second;

		Trader& buyer = traders[bids[i].first];
		Trader& seller = traders[asks[j].first];
		if (bidOrder.price >= askOrder.price) {
			double tradePrice = 0.5 * (bidOrder.price + askOrder.price);
			//currentPrice = tradePrice; // last traded price
			//std::cout << currentPrice << "\n";
			//priceHistory.push_back(tradePrice);
			double tradeQty = std::min(bidOrder.quantity, askOrder.quantity);
			double tradeValue = tradePrice * tradeQty;

			totalValue += tradeValue;
			totalQty += tradeQty;

			// friction (e.g., transaction fee)
			//double fee = tradeValue * 0.3;
			double fee = 0.0;

			buyer.wealth -= tradeValue + fee;
			seller.wealth += tradeValue - fee;

			buyer.holdings += tradeQty;
			seller.holdings -= tradeQty;

			bidOrder.quantity -= tradeQty;
			askOrder.quantity -= tradeQty;

			if (bidOrder.quantity <= 1e-4) {
				bidOrder.isMatched = true;
				++i;
			} 
			if (askOrder.quantity <= 1e-4) {
				askOrder.isMatched = true;
				++j;
			}
		}
		else break;
	}
	if (totalQty > 0) {
		currentPrice = totalValue / totalQty;
		//currentPrice += (basePrice - currentPrice) * 0.01; // magnet to base price
		priceHistory.push_back(currentPrice);
	}

	//getOrderBook(bids, asks, 10);
	
	//for (auto& t : traders) {
	//	t.orders.erase(
	//		std::remove_if(t.orders.begin(), t.orders.end(),
	//			[](const MarketOrder& o) { return o.isMatched; }),
	//		t.orders.end()
	//	);
	//}
}
void Market::redistributeWealth(std::vector<Trader>& traders)
{
	static const double baseRedistribution = 0.0035;
	//static const double baseRedistribution = 1.0;

	double totalWealth = 0.0;
	for (auto& t : traders) totalWealth += t.wealth;
	double meanWealth = totalWealth / traders.size();

	for (auto& t : traders) {
		double redistributionRate = baseRedistribution;
		if (t.wealth >= 10.0 * meanWealth) {
			// For very wealthy traders, apply a stronger redistribution
			redistributionRate = baseRedistribution + 0.00005 * std::pow(log10(t.wealth / meanWealth), 10);
		}

		t.wealth = (1.0 - redistributionRate) * t.wealth + redistributionRate * meanWealth;
	}
}

void Market::findMarketStats(
	const std::vector<Trader>& traders, const size_t months,
	const std::vector<double>& priceHistory
)
{
	size_t tradersCount = traders.size();

	auto sumWealth = [](auto begin, auto end) {
		return std::accumulate(
			begin, end, 0.0,
			[](double sum, const Trader& t) { return sum + t.wealth; }
		);
	};

	auto sumHoldings = [](auto begin, auto end) {
		return std::accumulate(
			begin, end, 0.0,
			[](double sum, const Trader& t) { return sum + t.holdings; }
		);
	};

	size_t top1count = std::max<size_t>(1, tradersCount / 100);
	size_t top10count = std::max<size_t>(1, tradersCount / 10);

	double totalWealth = sumWealth(traders.begin(), traders.end());
	double totalHoldings = sumHoldings(traders.begin(), traders.end());
	double mean = totalWealth / tradersCount;
	double median = (tradersCount % 2 == 0)
		? (traders[tradersCount / 2 - 1].wealth + traders[tradersCount / 2].wealth) / 2.0
		: traders[tradersCount / 2].wealth;
	double top1 = sumWealth(traders.begin(), traders.begin() + top1count);
	double top10 = sumWealth(traders.begin(), traders.begin() + top10count);
	double bottom50 = sumWealth(traders.begin() + tradersCount / 2, traders.end());
	double best = traders.front().wealth;
	double worst = traders.back().wealth;
	double bestPercent = best / totalWealth * 100.0;
	double endPrice = priceHistory.empty() ? 0.0 : priceHistory.back();

	double investedWealth = totalHoldings * endPrice;
	double investedPercent = (investedWealth / (totalWealth)) * 100.0;

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3);

	oss << "\n---Zero-Sum Market Simulation---\n";
	oss << "Traders: " << tradersCount << ", Months: " << months << "\n";
	oss << "Total wealth: " << totalWealth << "\n";
	oss << "Total holdings: " << totalHoldings << "\n";
	oss << "Mean: " << mean << ", Median: " << median << "\n";
	oss << "Best: " << best << " (" << bestPercent << "% of total)\n";
	oss << "Worst: " << worst << "\n";
	oss << "Top 1% share: " << (top1 / totalWealth * 100.0) << "%\n";
	oss << "Top 10% share: " << (top10 / totalWealth * 100.0) << "%\n";
	oss << "Bottom 50% share: " << (bottom50 / totalWealth * 100.0) << "%\n";
	oss << "End price: " << endPrice << "\n";
	oss << "Wealth invested in security: " << investedWealth
		<< " (" << investedPercent << "% of total)\n\n";

	oss << "Wealth by percentile:\n";
	const size_t step = 5; // print every N%
	for (size_t p = 0; p <= 100; p += step) {
		size_t idx = std::clamp<size_t>(
			static_cast<size_t>(p / 100.0 * (tradersCount - 1)),
			0, tradersCount - 1
		);
		oss << std::setw(3) << 100 - p << "% : " << std::fixed << std::setprecision(3)
			<< traders[idx].wealth << "\n";
	}

	std::cout << oss.str();

	/*std::ofstream file("price_history.csv");
	file << "Month,Price\n";
	for (size_t i = 0; i < priceHistory.size(); ++i) {
		file << i << "," << priceHistory[i] << "\n";
	}
	file.close();*/
}
void Market::getOrderBook(Orders bids, Orders asks, size_t N)
{
	Orders topAsks;

	// collect last N unmatched asks
	for (auto it = asks.begin(); it != asks.end(); ++it) {
		if (!it->second.isMatched) {
			topAsks.push_back(*it);
			if (N > 0 && topAsks.size() >= N) break;
		}
	}

	// print them in descending order (highest price first)
	std::reverse(topAsks.begin(), topAsks.end());

	std::cout << std::fixed << std::setprecision(3);

	for (auto& entry : topAsks) {
		const auto& order = entry.second;
		std::cout << "ASK | Trader " << std::setw(5) << entry.first
			<< " | Price: " << std::setw(8) << order.price
			<< " | Qty: " << std::setw(8) << order.quantity << "\n";
	}

	// Print bids first (highest price at top)
	size_t count = 0;
	for (auto& entry : bids) {
		const auto& order = entry.second;
		if (!order.isMatched) {
			std::cout << "BID | Trader " << std::setw(5) << entry.first
				<< " | Price: " << std::setw(8) << order.price
				<< " | Qty: " << std::setw(8) << order.quantity << "\n";
			if (N > 0 && ++count >= N) break;
		}
	}

	std::cout << "\n";
}
