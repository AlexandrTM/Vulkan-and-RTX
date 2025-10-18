#pragma once

struct Security
{
	size_t id;
	double price;
	double lotStep;   // minimal tradable unit
	double priceStep; // minimal price increment
};

struct MarketOrder
{
	double price;
	double quantity;
	bool isBuy;
	bool isMatched = false;
};

struct Trader
{
	double wealth = 0.0;
	double skill = 0.0;
	double positionLimit = 0.0;
	double holdings = 0.0;
	std::vector<MarketOrder> orders;
};

namespace Market
{
	void simulateMarket(bool draw = false);
	std::vector<double> simulateSecurity(size_t tradersCount, size_t months, Security& security);
	void simulateOrderBook(
		std::vector<Trader>& traders,
		Security& security,
		const double& basePrice,
		std::vector<double>& priceHistory
	);
	void redistributeWealth(std::vector<Trader>& traders);
	void findMarketStats(
		const std::vector<Trader>& traders, const size_t months,
		const std::vector<double>& priceHistory
	);
	using Orders = std::vector<std::pair<size_t, MarketOrder>>;
	void getOrderBook(Orders bids, Orders asks, size_t N = 0);
}

